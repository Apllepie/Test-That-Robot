#include "occupancy_grid.h"
#include <cmath>
#include <QDebug>

OccupancyGrid::OccupancyGrid(float worldWidth, float worldHeight, float cellSize)
    : _cellSize(cellSize)
{
    _width = static_cast<int>(std::ceil(worldWidth / _cellSize));
    _height = static_cast<int>(std::ceil(worldHeight / _cellSize));
    _origin = QVector2D(-(_width * _cellSize) / 2.0f, -(_height * _cellSize) / 2.0f);
    _gridData.resize(_width * _height, 0);
}

void OccupancyGrid::clear()
{
    std::fill(_gridData.begin(), _gridData.end(), 0);
}

void OccupancyGrid::updateFromObstacles(const std::vector<std::unique_ptr<Object>>& primitives)
{
    clear(); // Очищаем карту

    // 1. Определяем размер робота для раздутия
    float maxRobotScale = 1.0f; 
    bool robotFound = false;

    for (const auto& obj : primitives) {
        if (obj->isRobot) {
            // Берем максимальный масштаб по осям (обычно X и Y равны)
            maxRobotScale = std::max(obj->getScale().x(), obj->getScale().y());
            robotFound = true;
            break; 
        }
    }

    // ТРЕБОВАНИЕ: "больше половины робота в 2 раза".
    // Базовый радиус меша робота примерно 0.4 (от центра).
    // Половина размера = 0.4 * scale.
    // Умножаем на 2 -> 0.8 * scale.
    // Добавим немного запаса (коэффициент 2.2 вместо 2.0), чтобы наверняка.
    float inflationRadiusWorld = (0.4f * maxRobotScale) * 1.5f;

    // --- ЭТАП 1: Растеризация препятствий (стены становятся 255) ---
    for (const auto& obj : primitives) {
        Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
        if (!obstacle) continue;

        QMatrix4x4 model = obstacle->getModelMatrix();
        bool invertible;
        QMatrix4x4 invModel = model.inverted(&invertible);
        if (!invertible) continue;

        auto vertices = obstacle->getGlobalVertices();
        if (vertices.empty()) {
            float maxSize = std::max(obstacle->getScale().x(), obstacle->getScale().y());
            vertices.push_back(QVector2D(obstacle->getX() - maxSize, obstacle->getY() - maxSize));
            vertices.push_back(QVector2D(obstacle->getX() + maxSize, obstacle->getY() + maxSize));
        }

        float minX = vertices[0].x(), maxX = vertices[0].x();
        float minY = vertices[0].y(), maxY = vertices[0].y();
        for (size_t i = 1; i < vertices.size(); ++i) {
            minX = std::min(minX, vertices[i].x());
            maxX = std::max(maxX, vertices[i].x());
            minY = std::min(minY, vertices[i].y());
            maxY = std::max(maxY, vertices[i].y());
        }

        int gridMinX, gridMinY, gridMaxX, gridMaxY;
        worldToGrid(QVector2D(minX, minY), gridMinX, gridMinY);
        worldToGrid(QVector2D(maxX, maxY), gridMaxX, gridMaxY);

        for (int y = gridMinY; y <= gridMaxY; ++y) {
            for (int x = gridMinX; x <= gridMaxX; ++x) {
                if (x < 0 || x >= _width || y < 0 || y >= _height) continue;
                QVector2D cellCenterWorld = gridToWorld(x, y);
                QVector4D localPoint4D = invModel * QVector4D(cellCenterWorld.x(), cellCenterWorld.y(), 0.0f, 1.0f);

                if (obstacle->isInside(localPoint4D.toVector2D())) {
                    setCell(x, y, 255); // Ставим стену
                }
            }
        }
    }

    // --- ЭТАП 2: Раздутие (Inflation) вокруг препятствий ---
    // Переводим радиус из мировых единиц в клетки
    int inflation_in_cells = static_cast<int>(std::ceil(inflationRadiusWorld / _cellSize));

    if (inflation_in_cells > 0) {
        // Копия данных, чтобы читать "чистые" стены
        std::vector<unsigned char> original_grid_data = _gridData;
        
        // Значения стоимости для градиента
        const unsigned char COST_NEAR_WALL = 254; // Почти стена
        const unsigned char COST_AT_EDGE = 50;   // Край опасной зоны

        float radius_f = static_cast<float>(inflation_in_cells);

        // Проходим по всей карте
        for (int y = 0; y < _height; ++y) {
            for (int x = 0; x < _width; ++x) {
                // Если находим стену в оригинальной карте
                if (original_grid_data[y * _width + x] == 255) {

                    // Рисуем круг вокруг этой точки стены
                    for (int iy = -inflation_in_cells; iy <= inflation_in_cells; ++iy) {
                        for (int ix = -inflation_in_cells; ix <= inflation_in_cells; ++ix) {
                            
                            // Расстояние от точки стены до текущей проверяемой точки
                            float dist = std::sqrt(static_cast<float>(ix * ix + iy * iy));

                            // Если вышли за пределы радиуса раздутия - пропускаем
                            if (dist > radius_f) continue;

                            int nx = x + ix;
                            int ny = y + iy;

                            // Проверяем границы карты
                            if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
                                int idx = ny * _width + nx;
                                
                                // Не перезаписываем сами стены
                                if (_gridData[idx] == 255) continue;

                                // Считаем градиент: чем ближе к стене (dist=0), тем выше стоимость
                                float factor = 1.0f - (dist / radius_f);
                                unsigned char gradientValue = static_cast<unsigned char>(
                                    COST_AT_EDGE + (COST_NEAR_WALL - COST_AT_EDGE) * factor
                                );

                                // Записываем МАКСИМАЛЬНОЕ значение (чтобы наложения стен не уменьшали опасность)
                                if (gradientValue > _gridData[idx]) {
                                    setCell(nx, ny, gradientValue);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
// --- РЕАЛИЗАЦИЯ НЕДОСТАЮЩИХ ФУНКЦИЙ ---

bool OccupancyGrid::worldToGrid(const QVector2D& worldPos, int& gridX, int& gridY) const
{
    gridX = static_cast<int>(std::floor((worldPos.x() - _origin.x()) / _cellSize));
    gridY = static_cast<int>(std::floor((worldPos.y() - _origin.y()) / _cellSize));
    return (gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height);
}

QVector2D OccupancyGrid::gridToWorld(int gridX, int gridY) const
{
    float worldX = _origin.x() + (gridX + 0.5f) * _cellSize;
    float worldY = _origin.y() + (gridY + 0.5f) * _cellSize;
    return QVector2D(worldX, worldY);
}

void OccupancyGrid::setCell(int gridX, int gridY, unsigned char value)
{
    if (gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height) {
        _gridData[gridY * _width + gridX] = value;
    }
}

bool OccupancyGrid::isOccupied(int gridX, int gridY) const
{
    if (gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height) {
        return _gridData[gridY * _width + gridX] > OCCUPANCY_THRESHOLD;
    }
    return true; // За пределами карты - стена
}

bool OccupancyGrid::isOccupied(const QVector2D& worldPos) const
{
    int gridX, gridY;
    if (worldToGrid(worldPos, gridX, gridY)) {
        return isOccupied(gridX, gridY);
    }
    return true; // За пределами карты - стена
}

unsigned char OccupancyGrid::getCellValue(int gridX, int gridY) const
{
    if (gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height) {
        return _gridData[gridY * _width + gridX];
    }
    return 255;
}

std::vector<std::pair<QVector2D, unsigned char>> OccupancyGrid::getOccupiedCellWorldPositions() const
{
    std::vector<std::pair<QVector2D, unsigned char>> positions;
    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            if (isOccupied(x, y)) {
                positions.push_back({gridToWorld(x, y), getCellValue(x, y)});
            }
        }
    }
    return positions;
}
