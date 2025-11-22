#ifndef OBSTACLE_H
#define OBSTACLE_H  

#include <cerrno>
#include <QMatrix4x4>
#include <object.h>
#include <qmath.h>

class Obstacle: public Object
{

    private: 
    float _width;
    float _height;
public:
    Obstacle();
    Obstacle(Mesh *mesh, float width, float height);
    float getWidth() const { return _width; }
    float getHeight() const { return _height; }
    QString getType() const override { return "obstacle"; }
    std::vector<QVector2D> getGlobalVertices() const;
    virtual bool isInside(QVector2D localPoint) const ;
};

#endif
