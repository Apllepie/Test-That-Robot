#include "ebo.h"


EBO::EBO() {}

EBO::EBO(GLuint * indices, GLsizeiptr size)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    f->glGenBuffers(1,&ID);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);

}

EBO::~EBO()
{

}

void EBO::Bind()
{
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ID);
}

void EBO::Unbind()
{
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void EBO::Delete()
{
    f->glDeleteBuffers(1, &ID);
}
