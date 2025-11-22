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
    QVector3D _scale = QVector3D(1.0f, 1.0f, 1.0f);
    size_t _id;
    



    QMatrix4x4 _modelMatrix;
    Mesh *  _mesh;

    void updateModelMatrixFromParameters();
    void updateParametersFromModelMatrix();

public:
    Object() = default;
    Object(Mesh *mesh);
    virtual ~Object() = default;

    bool isRobot = false;

    virtual void update(float){};
    virtual void start(){};
    virtual void stop(){};

    //FUNCTIONS TO TRANSFORM THE OBJECT
    virtual void Translate(float dx, float dy, float dz);
    virtual void Translate(QVector3D dir);
    virtual void Scale(float dx, float dy, float dz);
    virtual void Scale(QVector3D factor);
    virtual void Rotate(float theta, bool xAxis, bool yAxis, bool zAxis);
    //GETTERS AND SETTERS
    virtual QString getType() const { return "object"; }
    float getX() const { return _x; }
    float getY() const { return _y; }
    float getAngle() const { return _angle; }   
    QVector3D getScale() const { return _scale; }
    QMatrix4x4 getModelMatrix() const {return _modelMatrix;}
    void setModelMatrix(const QMatrix4x4 &newModelMatrix) {_modelMatrix = newModelMatrix;}
    Mesh* getMesh() const {return _mesh; }
    size_t getId() const {return _id; }
    void setId(size_t id) {_id = id; }
};

#endif // OBJECT_H
