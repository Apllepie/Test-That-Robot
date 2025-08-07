#ifndef OBJECT_H
#define OBJECT_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>


class Object
{
public:
    enum class type {BOX, TRIANGLE};

    Object();
    Object(std::vector<GLfloat> vertices, std::vector<GLuint> indices, std::vector<GLfloat> Color );
    Object(type TYPE, std::vector<GLfloat> Color);

    QMatrix4x4 modelMatrix;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::vector<GLfloat> Color;

    //fun
    void initialize();
    void addModel(GLuint &uniID);

private:
    QOpenGLExtraFunctions *f;
    void Box();
    void Triangle();
    void addColor();

};

#endif // OBJECT_H
