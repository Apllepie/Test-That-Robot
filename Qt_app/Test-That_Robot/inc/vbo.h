#ifndef VBO_H
#define VBO_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>

//Vertices buffer object
class VBO
{
public:
    VBO();
    ~VBO();

    GLuint ID;

    //fun
    void Init(GLfloat * vertices, GLsizeiptr size );
    void Bind();
    void Unbind();
    void Delete();


private:
    QOpenGLExtraFunctions *f;
};

#endif // VBO_H
