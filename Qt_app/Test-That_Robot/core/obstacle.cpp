#include "obstacle.h"

Obstacle::Obstacle()
{
}

Obstacle::Obstacle(Mesh *mesh, float width, float height) : Object(mesh)
{
    _x = 0.0f;
    _y = 0.0f;
    _width = width;
    _height = height;
    Object::Scale(QVector3D(width,height, 0.1f));
}

std::vector<QVector2D> Obstacle::getGlobalVertices() const
{
    // Локальные координаты углов прямоугольника (до всех трансформаций)
    float w = 0.5f;
    float h = 0.5f;
    std::vector<QVector4D> localVertices = {
        QVector4D(-w, -h, 0, 1.0f),
        QVector4D( w, -h, 0, 1.0f),
        QVector4D( w,  h, 0, 1.0f),
        QVector4D(-w,  h, 0, 1.0f)
    };

    std::vector<QVector2D> globalVertices;
    const QMatrix4x4& model = getModelMatrix();

    for(const auto& v : localVertices) {
        QVector4D globalV = model * v;
        globalVertices.push_back(globalV.toVector2D());
    }

    return globalVertices;
}

bool Obstacle::isInside(QVector2D localPoint) const
{
    // float halfWidth = _width / 2.0f;
    // float halfHeight = _height / 2.0f;

    // return (localPoint.x() >= -halfWidth && localPoint.x() <= halfWidth &&
    //         localPoint.y() >= -halfHeight && localPoint.y() <= halfHeight);
    return (std::abs(localPoint.x()) <= 0.5f && 
            std::abs(localPoint.y()) <= 0.5f);
}