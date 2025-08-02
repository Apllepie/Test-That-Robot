#include "vao.h"

VAO::VAO() {
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    f->glGenVertexArrays(1, &ID);

}

VAO::~VAO()
{

}

void VAO::linkAttribut(VBO VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void * offset)
{
    VBO.Bind();
    f->glVertexAttribPointer(layout, numComponents, type,GL_FALSE,stride, offset);
    f->glEnableVertexAttribArray(layout);
}

void VAO::Bind()
{
    f->glBindVertexArray(ID);

}

void VAO::Unbind()
{
    f->glBindVertexArray(0);
}


void VAO::Delete()
{
    f->glDeleteVertexArrays(1, &ID);
}
