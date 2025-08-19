#ifndef OBJECT_H
#define OBJECT_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>

#include "shaderclass.h"
#include "mesh.h"


class Object
{
public:
    Object();
    Object(Mesh *mesh);
    ~Object();




    QMatrix4x4 modelMatrix;
    QMatrix4x4 scaleMatrix;
    QMatrix4x4 transMatrix;
    QMatrix4x4 ZrotateMatrix;
    QMatrix4x4 XrotateMatrix;
    QMatrix4x4 YrotateMatrix;


    std::vector<GLfloat> Color;

    Mesh *mesh;
    //fun
    void initialize();
    void addModel(GLuint &uniID);
    void Draw(Shader *shader);

private:
    QOpenGLExtraFunctions *f;


    GLuint uniID;


};

#endif // OBJECT_H
