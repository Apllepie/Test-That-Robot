#include "vbo.h"


VBO::VBO() {}


VBO::~VBO()
{

}

void VBO::Init(const void* data, GLsizeiptr size, GLenum usage)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    f->glGenBuffers(1,&ID);
    f->glBindBuffer(GL_ARRAY_BUFFER, ID);
    f->glBufferData(GL_ARRAY_BUFFER, size, data, usage);

}

void VBO::Update(const void* data, GLsizeiptr size)
{
    Bind();
    f->glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    Unbind();
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
