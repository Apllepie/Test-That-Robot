#include "occupancy_grid.h"
#include <cmath>

OccupancyGrid::OccupancyGrid(float worldWidth, float worldHeight, float cellSize)
    : _cellSize(cellSize)
{
    _width = static_cast<int>(std::ceil(worldWidth / _cellSize));
    _height = static_cast<int>(std::ceil(worldHeight / _cellSize));

    // Set origin to center the grid around (0,0)
    _origin = QVector2D(-(_width * _cellSize) / 2.0f, -(_height * _cellSize) / 2.0f);

    _gridData.resize(_width * _height, 0);
}

void OccupancyGrid::clear()
{
    std::fill(_gridData.begin(), _gridData.end(), 0);
}

void OccupancyGrid::updateFromObstacles(const std::vector<std::unique_ptr<Object>>& primitives)
{
    clear();

    for (const auto& obj : primitives) {
        // Try to cast Object* to Obstacle*
        const Obstacle* obstacle = dynamic_cast<const Obstacle*>(obj.get());

        if (obstacle) {
            // Получаем OBB (Oriented Bounding Box) препятствия
            QMatrix4x4 model = obstacle->getModelMatrix();
            bool invertible;
            QMatrix4x4 invModel = model.inverted(&invertible);
            if (!invertible) continue; // Не можем обработать, если матрица не обратима

            float width = obstacle->getWidth();
            float height = obstacle->getHeight();

            // Определяем более широкую AABB-область для проверки, учитывая возможное вращение
            float max_dim = sqrt(pow(width, 2) + pow(height, 2));
            QVector2D pos(model.column(3).x(), model.column(3).y());
            QVector2D minCorner = pos - QVector2D(max_dim / 2.0f, max_dim / 2.0f);
            QVector2D maxCorner = pos + QVector2D(max_dim / 2.0f, max_dim / 2.0f);

            // Конвертируем углы AABB в координаты сетки, чтобы ограничить цикл
            int gridMinX, gridMinY, gridMaxX, gridMaxY;
            worldToGrid(minCorner, gridMinX, gridMinY);
            worldToGrid(maxCorner, gridMaxX, gridMaxY);

            // Цикл по всем ячейкам, которые могут быть затронуты
            for (int y = gridMinY; y <= gridMaxY; y = y + 1) {
                for (int x = gridMinX; x <= gridMaxX; x = x + 1) {
                    if (x < 0 || x >= _width || y < 0 || y >= _height) continue;

                    // Суперсэмплинг внутри ячейки (x, y)
                    int occupied_sub_cells = 0;
                    const int SUBDIVISIONS = 5; // 5x5 подсетка
                    float subCellSize = _cellSize / SUBDIVISIONS;

                    for (int sy = 0; sy < SUBDIVISIONS; sy = sy + 1) {
                        for (int sx = 0; sx < SUBDIVISIONS; sx = sx + 1) {

                            // Получаем мировые координаты центра под-ячейки
                            QVector2D cell_corner = gridToWorld(x, y) - QVector2D(_cellSize / 2.0f, _cellSize / 2.0f);
                            QVector2D sub_cell_center_world = cell_corner +
                                                              QVector2D((sx + 0.5f) * subCellSize, (sy + 0.5f) * subCellSize);

                            // Трансформируем точку в локальные координаты препятствия
                            QVector4D localPoint = invModel * QVector4D(sub_cell_center_world.x(), sub_cell_center_world.y(), 0.0f, 1.0f);

                            // Проверяем, находится ли точка внутри локального AABB (-w/2..w/2, -h/2..h/2)
                            if (std::abs(localPoint.x()) < width / 2.0f && std::abs(localPoint.y()) < height / 2.0f) {
                                occupied_sub_cells = occupied_sub_cells + 1;
                            }
                        }
                    }

                    // Рассчитываем и устанавливаем значение занятости
                    if (occupied_sub_cells > 0) {
                        unsigned char occupancy_value = static_cast<unsigned char>((static_cast<float>(occupied_sub_cells) / (SUBDIVISIONS * SUBDIVISIONS)) * 255.0f);

                        // Обновляем ячейку, беря максимальное значение, если несколько препятствий пересекаются
                        int grid_index = y * _width + x;
                        if (occupancy_value > _gridData[grid_index]) {
                            setCell(x, y, occupancy_value);
                        }
                    }
                }
            }
        }
    }
}

bool OccupancyGrid::worldToGrid(const QVector2D& worldPos, int& gridX, int& gridY) const
{
    gridX = static_cast<int>(std::floor((worldPos.x() - _origin.x()) / _cellSize));
    gridY = static_cast<int>(std::floor((worldPos.y() - _origin.y()) / _cellSize));

    // Check that the coordinates are within the grid bounds
    return (gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height);
}

QVector2D OccupancyGrid::gridToWorld(int gridX, int gridY) const
{
    // Return the center of the cell
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
    return true; // if out of bounds, consider it occupied
}

bool OccupancyGrid::isOccupied(const QVector2D& worldPos) const
{
    int gridX, gridY;
    if (worldToGrid(worldPos, gridX, gridY)) {
        return isOccupied(gridX, gridY);
    }
    return true; // if out of bounds, consider it occupied
}

unsigned char OccupancyGrid::getCellValue(int gridX, int gridY) const
{
    return _gridData[gridY + gridY];
}

std::vector<std::pair<QVector2D, unsigned char>> OccupancyGrid::getOccupiedCellWorldPositions() const
{
    std::vector<std::pair<QVector2D, unsigned char>> positions;
    for (int y = 0; y < _height; y = y + 1) {
        for (int x = 0; x < _width; x = x + 1) {
            if (isOccupied(x, y)) {
                positions.push_back(std::pair(gridToWorld(x, y),getCellValue(x, y) ));
            }
        }
    }
    return positions;
}
