#include "opengl.h"
#include <GL/gl.h>
#include <QOpenGLContext>
#include <QDebug>
#include <math.h>

OpenGl::OpenGl(QWidget *parent): QOpenGLWidget(parent)
{

}

OpenGl::~OpenGl()
{


}

void OpenGl::initializeGL()
{
    initializeOpenGLFunctions();
    initDebug();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    qDebug() << "OpenGL initialized";

    camera.Init();
    scene.initialize();
   
}

void OpenGl::resizeGL(int w, int h)
{
    const float dpr = devicePixelRatioF();
    scene.resize(int(w * dpr), int(h * dpr));
}
void OpenGl::paintGL()
{
    clearError();
    camera.Activate(scene.defaultShader);
    scene.paint(camera);
    //scene.paint(camera);


    checkError();
}

void OpenGl::wheelEvent(QWheelEvent *event)
{
        float delta = event->angleDelta().y() / 120.0f;
        camera.moveCloser_Away(-delta);
        update();
}

void OpenGl::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        rightMousePress = true;
        lastMousePos  = event->pos();
    }
    if(event->button() == Qt::LeftButton) {
        leftMousePress = true;
        lastMousePos = event->pos();

        const float dpr = devicePixelRatioF();
        const int px = int(lastMousePos.x() * dpr);
        const int py = int((height() - lastMousePos.y()) * dpr);

        auto pixel = scene.picking.readPixel(px, py);
        GLuint pickedID = pixel.r;
        if (pickedID > 0) scene.selectObject(pickedID - 1);
        else scene.selectObject(-1);

            qDebug() <<"mouse pos:" <<lastMousePos <<"pick" << pickedID << "\n";
            pixel.print();
        }
    update();
    }

void OpenGl::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        rightMousePress = false;
    }
    if(event->button() == Qt::LeftButton){
        leftMousePress = false;
    }
}

void OpenGl::mouseMoveEvent(QMouseEvent *event)
{
    if(rightMousePress){
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();
        camera.Move(delta);
    }

    update();
}

 void OpenGl::clearError()
{
    while(glGetError() != GL_NO_ERROR);
}

 void OpenGl::checkError()
{
     while (GLenum err = glGetError() ){
        qDebug()  <<"[OPENGL ERROR] ("<< err <<")\n";
     }
}

void OpenGl::glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    Q_UNUSED(length);
    Q_UNUSED(userParam);

    QString src;
    switch (source) {
    case GL_DEBUG_SOURCE_API: src = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: src = "Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: src = "Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: src = "Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION: src = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER: src = "Other"; break;
    }

    QString tp;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: tp = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tp = "Deprecated Behavior"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tp = "Undefined Behavior"; break;
    case GL_DEBUG_TYPE_PORTABILITY: tp = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: tp = "Performance"; break;
    case GL_DEBUG_TYPE_MARKER: tp = "Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP: tp = "Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP: tp = "Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER: tp = "Other"; break;
    }

    QString sev;
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: sev = "HIGH"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: sev = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW: sev = "LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: sev = "NOTIFICATION"; break;
    }

    qDebug().noquote() << QString("[OpenGL DEBUG] %1 | %2 | %3 | ID %4 | %5")
                              .arg(src)
                              .arg(tp)
                              .arg(sev)
                              .arg(id)
                              .arg(message);
}

void OpenGl::initDebug()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGl::glDebugOutput, nullptr);
}
