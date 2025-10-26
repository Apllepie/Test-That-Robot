#ifndef OBJECT_H
#define OBJECT_H


#include <cerrno>
#include <QMatrix4x4>
#include <float.h>
#include <memory>

#include "renderer/mesh.h"




class Object
{
protected:
    float _x = 0.0f;
    float _y = 0.0f;
    float _angle = 0.0f;
    float _scale = 1.0f;
    size_t _id;
    



    QMatrix4x4 _modelMatrix;
    Mesh * _mesh;

    void updateModelMatrixFromPosition();
    void updatePositionFromModelMatrix();

public:
    Object() = default;
    Object(Mesh *mesh);
    virtual ~Object() = default;

    bool isRobot = false;

    virtual void update(float){};
    virtual void start(){};
    virtual void stop(){};

    //FUNCTIONS TO TRANSFORM THE OBJECT
    void Translate(float dx, float dy, float dz);
    void Translate(QVector3D dir);
    void Scale(float x, float y, float z);
    void Rotate(float theta, bool xAxis, bool yAxis, bool zAxis);

    QMatrix4x4 getModelMatrix() const {return _modelMatrix;}
    void setModelMatrix(const QMatrix4x4 &newModelMatrix) {_modelMatrix = newModelMatrix;}
    Mesh* getMesh() const {return _mesh; }
};

#endif // OBJECT_H
