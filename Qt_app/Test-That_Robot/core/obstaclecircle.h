#ifndef OBSTACLECIRCLE_H
#define OBSTACLECIRCLE_H

#include "obstacle.h"
#include "vector"
#include <cmath>

class ObstacleCircle :public Obstacle
{
private:
    float _radius;
    int _nSides;
public:
    ObstacleCircle();
    ObstacleCircle(float radius, int nSides, Mesh *mesh);

    bool isInside(QVector2D localPoint) const override;
    std::shared_ptr<Mesh> mesh;

    QString getType() const override {
        if(_nSides == 3) return "obstacle_triangle";
        else
        return "obstacle_circle"; }
};

#endif // OBSTACLECIRCLE_H
