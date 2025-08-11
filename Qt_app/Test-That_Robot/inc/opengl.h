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


class OpenGl : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit OpenGl(QWidget *parent = nullptr);
    ~OpenGl();

protected:
    void initializeGL() ;
    void resizeGL(int w, int h) ;
    void paintGL() ;
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void clearError();
    void checkError();


    Scene scene;
    Camera camera;

    bool rightMousePress = false;
    bool leftMousePress = false;
    QPoint lastMousePos;
    Object object;

private:
    static void APIENTRY glDebugOutput(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const GLchar *message,
                                       const void *userParam);
    void initDebug();


};

#endif // OPENGL_H

