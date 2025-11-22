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
    clear(); // Всегда начинаем с чистой сетки

    // --- ЭТАП 1: Универсальная растеризация ЛЮБЫХ препятствий ---
    for (const auto& obj : primitives) {
        // Проверяем, является ли объект препятствием (имеет реализацию isInside)
        // dynamic_cast здесь не нужен, если все препятствия наследуются от Obstacle
        Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
        if (!obstacle) {
            continue;
        }

        // Получаем матрицу модели и ее инверсию для преобразования координат
        QMatrix4x4 model = obstacle->getModelMatrix();
        bool invertible;
        QMatrix4x4 invModel = model.inverted(&invertible);
        if (!invertible) {
            continue; // Пропускаем объект, если его матрица вырождена
        }

        // Находим AABB (осе-ориентированный ограничивающий прямоугольник) объекта,
        // чтобы не проверять каждую ячейку на всей карте.
        auto vertices = obstacle->getGlobalVertices();
        if (vertices.empty()) {
            // Для объектов без вершин (например, идеальный круг) можно взять AABB из позиции и размера
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

        // Конвертируем AABB в координаты сетки
        int gridMinX, gridMinY, gridMaxX, gridMaxY;
        worldToGrid(QVector2D(minX, minY), gridMinX, gridMinY);
        worldToGrid(QVector2D(maxX, maxY), gridMaxX, gridMaxY);

        // Проходимся по всем ячейкам внутри AABB этого препятствия
        for (int y = gridMinY; y <= gridMaxY; ++y) {
            for (int x = gridMinX; x <= gridMaxX; ++x) {
                // Пропускаем ячейки за пределами сетки
                if (x < 0 || x >= _width || y < 0 || y >= _height) {
                    continue;
                }

                // Получаем центр ячейки в мировых координатах
                QVector2D cellCenterWorld = gridToWorld(x, y);

                // Преобразуем центр ячейки в локальные координаты препятствия
                QVector4D localPoint4D = invModel * QVector4D(cellCenterWorld.x(), cellCenterWorld.y(), 0.0f, 1.0f);

                // --- ГЛАВНАЯ ЛОГИКА ---
                // "Спрашиваем" у самого объекта, находится ли эта точка внутри его формы.
                // Благодаря полиморфизму, вызовется правильная реализация:
                // для прямоугольника - проверка границ, для круга - проверка радиуса и т.д.
                if (obstacle->isInside(localPoint4D.toVector2D())) {
                    setCell(x, y, 255);
                }
            }
        }
    }

    // --- ЭТАП 2: "Раздувание" для C-Space (остается без изменений, он универсален) ---
    const float ROBOT_RADIUS = 0.6f;
    int inflation_in_cells = static_cast<int>(std::ceil(ROBOT_RADIUS / _cellSize));

    if (inflation_in_cells > 0) {
        // Копируем данные, чтобы читать стены из оригинала, а писать градиент в актуальную карту
        std::vector<unsigned char> original_grid_data = _gridData;

        // Параметры градиента
        const unsigned char COST_NEAR_WALL = 200; // Значение вплотную к стене (максимум градиента)
        const unsigned char COST_AT_EDGE = 127;    // Значение на самом краю радиуса (минимум)

        // Преобразуем радиус во float для расчетов
        float radius_f = static_cast<float>(inflation_in_cells);

        for (int y = 0; y < _height; ++y) {
            for (int x = 0; x < _width; ++x) {
                // Если в оригинальной карте здесь была стена
                if (original_grid_data[y * _width + x] == 255) {

                    for (int iy = -inflation_in_cells; iy <= inflation_in_cells; ++iy) {
                        for (int ix = -inflation_in_cells; ix <= inflation_in_cells; ++ix) {

                            // Считаем расстояние от центра препятствия до текущей точки ядра
                            float dist = std::sqrt(static_cast<float>(ix * ix + iy * iy));

                            // Если вышли за радиус круга — пропускаем
                            if (dist > radius_f) {
                                continue;
                            }

                            int nx = x + ix;
                            int ny = y + iy;

                            if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
                                int idx = ny * _width + nx;

                                // Не перезаписываем сами стены (255)
                                if (_gridData[idx] == 255) {
                                    continue;
                                }

                                // --- РАСЧЕТ ГРАДИЕНТА ---
                                // factor = 1.0 (у стены) -> 0.0 (на краю радиуса)
                                float factor = 1.0f - (dist / radius_f);

                                // Линейная интерполяция: от 10 до 127
                                unsigned char gradientValue = static_cast<unsigned char>(
                                    COST_AT_EDGE + (COST_NEAR_WALL - COST_AT_EDGE) * factor
                                    );

                                // --- ВАЖНЫЙ МОМЕНТ ---
                                // Поскольку одна ячейка может попасть в радиус действия
                                // нескольких стен, мы всегда должны оставлять МАКСИМАЛЬНОЕ значение.
                                // Это гарантирует, что ближе к любой стене опасность выше.
                                if (gradientValue > _gridData[idx]) {
                                    setCell(nx, ny, gradientValue);
                                    // Или напрямую: _gridData[idx] = gradientValue;
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
