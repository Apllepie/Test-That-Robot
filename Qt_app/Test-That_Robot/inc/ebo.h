#ifndef EBO_H
#define EBO_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>

//elements buffer object
class EBO
{
public:
    EBO();
    EBO(GLuint * indices, GLsizeiptr size );
    ~EBO();

    GLuint ID;

    //fun
    void Bind();
    void Unbind();
    void Delete();


private:
    QOpenGLExtraFunctions *f;
};

#endif // EBO_H
