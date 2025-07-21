#include "opengl.h"


#include "opengl.h"
#include <QOpenGLContext>
#include <QDebug>

OpenGl::OpenGl(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // Настройки по умолчанию
}

OpenGl::~OpenGl()
{
    // Очистка, если нужна
}

void OpenGl::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    qDebug() << "OpenGL initialized";
}

void OpenGl::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
}

void OpenGl::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    // Здесь можно рисовать
}
