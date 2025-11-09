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
   clear(); // Всегда начинаем с чистой сетки

    // --- ЭТАП 1: Надежная растеризация препятствий с помощью суперсэмплинга ---
    for (const auto& obj : primitives) {
        const Obstacle* obstacle = dynamic_cast<const Obstacle*>(obj.get());
        if (obstacle) {
            QMatrix4x4 model = obstacle->getModelMatrix();
            bool invertible;
            QMatrix4x4 invModel = model.inverted(&invertible);
            if (!invertible) continue;

            float width = obstacle->getWidth();
            float height = obstacle->getHeight();
            
            // Определяем область для проверки, немного расширив ее
            float max_dim = sqrt(pow(width, 2) + pow(height, 2));
            QVector2D pos(model.column(3).x(), model.column(3).y());
            QVector2D minCorner = pos - QVector2D(max_dim, max_dim);
            QVector2D maxCorner = pos + QVector2D(max_dim, max_dim);

            int gridMinX, gridMinY, gridMaxX, gridMaxY;
            worldToGrid(minCorner, gridMinX, gridMinY);
            worldToGrid(maxCorner, gridMaxX, gridMaxY);

            // Проходим по всем ячейкам, которые могут быть затронуты
            for (int y = gridMinY; y <= gridMaxY; ++y) {
                for (int x = gridMinX; x <= gridMaxX; ++x) {
                    if (x < 0 || x >= _width || y < 0 || y >= _height) continue;

                    bool cell_is_occupied = false;
                    const int SUBDIVISIONS = 3; // Проверяем сетку 3x3 внутри ячейки
                    float subCellStep = _cellSize / SUBDIVISIONS;

                    // Суперсэмплинг: проверяем несколько точек внутри ячейки
                    for (int sy = 0; sy < SUBDIVISIONS; ++sy) {
                        for (int sx = 0; sx < SUBDIVISIONS; ++sx) {
                            QVector2D sub_point_world = gridToWorld(x,y) - QVector2D(_cellSize/2.0, _cellSize/2.0) + 
                                                        QVector2D((sx + 0.5f) * subCellStep, (sy + 0.5f) * subCellStep);

                            QVector4D localPoint = invModel * QVector4D(sub_point_world.x(), sub_point_world.y(), 0.0f, 1.0f);
                            
                            // Проверяем, находится ли суб-точка внутри локального AABB препятствия
                            if (std::abs(localPoint.x()) < width / 2.0f && std::abs(localPoint.y()) < height / 2.0f) {
                                setCell(x, y, 255);
                                cell_is_occupied = true;
                                break; // Ячейка точно занята, можно переходить к следующей
                            }
                        }
                        if (cell_is_occupied) break;
                    }
                }
            }
        }
    }

    // --- ЭТАП 2: "Раздувание" препятствий (создание C-Space) ---
    const float ROBOT_RADIUS = 0.4f; // Немного увеличим для надежности (диагональ робота ~0.56)
    int inflation_in_cells = static_cast<int>(std::ceil(ROBOT_RADIUS / _cellSize));
    if (inflation_in_cells == 0) return;
    
    std::vector<unsigned char> original_grid_data = _gridData;
    
    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            if (original_grid_data[y * _width + x] > OCCUPANCY_THRESHOLD) {
                for (int iy = -inflation_in_cells; iy <= inflation_in_cells; ++iy) {
                    for (int ix = -inflation_in_cells; ix <= inflation_in_cells; ++ix) {
                        if (ix * ix + iy * iy > inflation_in_cells * inflation_in_cells) continue;
                        
                        int nx = x + ix;
                        int ny = y + iy;

                        if (nx >= 0 && nx < _width && ny >= 0 && ny < _height) {
                            setCell(nx, ny, 255);
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
