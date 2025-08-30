#ifndef OPENGL_H
#define OPENGL_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <GL/glext.h>
#include <QOpenGLContext>
#include <QDebug>
#include <math.h>

#include <QMatrix4x4>
#include <QVector3D>
#include <QTime>
#include <QMouseEvent>
#include <QWheelEvent>


#include "camera.h"
#include "scene.h"
#include "inputcontroller.h"


class OpenGl : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit OpenGl(QWidget *parent = nullptr);
    ~OpenGl();

    float getDpr();

protected:
    InputController input;
    Scene scene;


    float dpr;


    void initializeGL() ;
    void resizeGL(int w, int h) ;
    void paintGL();


    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);


private:
    QTimer *frameTimer;
    QElapsedTimer simTimer;
    float lastTime = 0.0f;


    //fun
    static void APIENTRY glDebugOutput(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const GLchar *message,
                                       const void *userParam);
    void initDebug();

    public slots:
    void addingBox();


};

#endif // OPENGL_H

