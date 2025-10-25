#ifndef OBJECT_H
#define OBJECT_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>
#include <float.h>

#include "shaderclass.h"
#include "mesh.h"

struct AABB{
    QVector3D min, max;

    AABB() : min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX){}


    void expand(const QVector3D & point){ //expanding bounce box, adding point
        min.setX(std::min(min.x(), point.x()));
        min.setY(std::min(min.y(), point.y()));
        min.setZ(std::min(min.z(), point.z()));

        max.setX(std::max(max.x(), point.x()));
        max.setY(std::max(max.y(), point.y()));
        max.setZ(std::max(max.z(), point.z()));

    }
    const QVector3D getCenter() {
        return (min + max) * 0.5f;
    }

    const QVector3D getSize(){
        return (max - min);
    }

    bool isValid(){
        return min.x() != FLT_MAX;
    }

};




class Object
{
public:
    std::vector<GLfloat> Color;

    Mesh *mesh;

    QMatrix4x4 modelMatrix;
    QMatrix4x4 scaleMatrix;
    QMatrix4x4 transMatrix;
    QMatrix4x4 ZrotateMatrix;
    QMatrix4x4 XrotateMatrix;
    QMatrix4x4 YrotateMatrix;

    bool isRobot = false;

    AABB cachedAABB;
    bool aabbDirty = true;



    Object();
    Object(Mesh *mesh);
    virtual ~Object() = default;

    virtual void update(float);
    virtual void start(int){};
    virtual void stop(){};

    //aabb fun
    AABB calculateAABB();
    AABB getAABB();
    void invalidateAABB(){aabbDirty = true;}
    //fun
    void initialize();
    void addModel(GLuint &uniID);
    void Draw(Shader *shader);
    void Translate(float x, float y, float z);
    void Scale(float x, float y, float z);
    void RotateZ(float theta);
    void RotateY(float theta);
    void RotateX(float theta);

     QMatrix4x4 updateModelMatrix();
private:
    QOpenGLExtraFunctions *f;
    GLuint uniID;

    //fun

};

#endif // OBJECT_H
