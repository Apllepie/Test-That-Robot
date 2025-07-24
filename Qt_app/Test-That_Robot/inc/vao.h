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
public:
    VAO();
    ~VAO();
    GLuint ID;

    void linkVBO(VBO VBO, GLuint layout);
    void Bind();
    void Unbind();
    void Delete();


private:
    QOpenGLExtraFunctions *f;
};

#endif // VAO_H
