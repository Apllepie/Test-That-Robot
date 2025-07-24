#include "vbo.h"


VBO::VBO() {}

VBO::VBO(GLfloat *vertices, GLsizeiptr size)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    f->glGenBuffers(1,&ID);
    f->glBindBuffer(GL_ARRAY_BUFFER, ID);
    f->glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

}

VBO::~VBO()
{

}

void VBO::Bind()
{
    f->glBindBuffer(GL_ARRAY_BUFFER,ID);
}

void VBO::Unbind()
{
    f->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void VBO::Delete()
{
    f->glDeleteBuffers(1, &ID);
}
