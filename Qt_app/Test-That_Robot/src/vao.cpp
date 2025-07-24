#include "../inc/vao.h"

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

void VAO::linkVBO(VBO VBO, GLuint layout)
{
    Bind();
    VBO.Bind();
    f->glVertexAttribPointer(layout, 3, GL_FLOAT,GL_FALSE, 0, (void*)0);
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
