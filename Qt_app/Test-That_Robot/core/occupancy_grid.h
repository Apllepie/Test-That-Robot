#ifndef OCCUPANCYGRID_H
#define OCCUPANCYGRID_H

#include <vector>
#include <object.h>
#include <obstacle.h>



class OccupancyGrid{
private:
    int _width;
    int _height;
    float _cellSize;
    QVector2D _origin; //grid corner

    std::vector<bool> _gridData;
public:
    OccupancyGrid(float worldW, float worldH, float cellSize);

    // grid Clear
    void clear();

    // update from obstucle
    void updateFromObstacles(const std::vector<std::unique_ptr<Object>>& primitives);

    bool worldToGrid(const QVector2D& worldPos, int& gridX, int& gridY) const;
    QVector2D gridToWorld(int gridX, int gridY) const;

    // set
    void setCell(int gridX, int gridY, bool isOccupied);
    bool isOccupied(int gridX, int gridY) const;
    bool isOccupied(const QVector2D& worldPos) const;

    // get
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    float getCellSize() const { return _cellSize; }

    // for visualisation
    std::vector<QVector2D> getOccupiedCellWorldPositions() const;
};
    


#endif // OCCUPANCYGRID_H
