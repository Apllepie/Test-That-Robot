#include "occupancy_grid.h"
#include <cmath> 

OccupancyGrid::OccupancyGrid(float worldWidth, float worldHeight, float cellSize)
    : _cellSize(cellSize)
{
    _width = static_cast<int>(std::ceil(worldWidth / _cellSize));
    _height = static_cast<int>(std::ceil(worldHeight / _cellSize));

    // Set origin to center the grid around (0,0)
    _origin = QVector2D(-(_width * _cellSize) / 2.0f, -(_height * _cellSize) / 2.0f);

    _gridData.resize(_width * _height, false);
}

void OccupancyGrid::clear()
{
    std::fill(_gridData.begin(), _gridData.end(), false);
}

void OccupancyGrid::updateFromObstacles(const std::vector<std::unique_ptr<Object>>& primitives)
{
    clear();

    for (const auto& obj : primitives) {
        // Try to cast Object* to Obstacle*
        const Obstacle* obstacle = dynamic_cast<const Obstacle*>(obj.get());

        if (obstacle) {
            // Get the AABB (Axis-Aligned Bounding Box) of the obstacle
            QMatrix4x4 model = obstacle->getModelMatrix();
            float width = obstacle->getWidth();
            float height = obstacle->getHeight();

            QVector2D pos(model.column(3).x(), model.column(3).y());
            QVector2D minCorner = pos - QVector2D(width / 2.0f, height / 2.0f);
            QVector2D maxCorner = pos + QVector2D(width / 2.0f, height / 2.0f);

            // Convert AABB corners to grid coordinates
            int gridMinX, gridMinY, gridMaxX, gridMaxY;
            worldToGrid(minCorner, gridMinX, gridMinY);
            worldToGrid(maxCorner, gridMaxX, gridMaxY);

            // Fill all cells inside the AABB
            for (int y = gridMinY; y <= gridMaxY; ++y) {
                for (int x = gridMinX; x <= gridMaxX; ++x) {
                    setCell(x, y, true);
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

void OccupancyGrid::setCell(int gridX, int gridY, bool isOccupied)
{
    if (gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height) {
        _gridData[gridY * _width + gridX] = isOccupied;
    }
}

bool OccupancyGrid::isOccupied(int gridX, int gridY) const
{
    if (gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height) {
        return _gridData[gridY * _width + gridX];
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

std::vector<QVector2D> OccupancyGrid::getOccupiedCellWorldPositions() const
{
    std::vector<QVector2D> positions;
    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            if (isOccupied(x, y)) {
                positions.push_back(gridToWorld(x, y));
            }
        }
    }
    return positions;
}
