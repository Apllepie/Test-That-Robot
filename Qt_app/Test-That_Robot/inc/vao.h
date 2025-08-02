#ifndef VAO_H
#define VAO_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>

#include "vbo.h"

//vertices array object
class VAO
{

private:
    QOpenGLExtraFunctions *f;

public:
    VAO();
    ~VAO();
    GLuint ID;

    //
    void Bind();
    void Unbind();
    void Delete();

    void linkAttribut(VBO VBO, GLuint layout,GLuint numComponents, GLenum type, GLsizeiptr stride, void * offset);




};

#endif // VAO_H
