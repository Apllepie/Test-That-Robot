#ifndef CAMERA_H
#define CAMERA_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <qmatrix4x4.h>
#include "shaderclass.h"
#include <math.h>

#define ZOOM_SPEED 0.1f

class Camera : public QOpenGLExtraFunctions
{
public:
    Camera();
    QMatrix4x4 view;
    QMatrix4x4 projection;

    void Init();
    void Activate(Shader *shader);
    void changeProjection(int w, int h, float angle, float start, float end);
    void moveCloser_Away(float delta);
    void Move(QPoint delta);
    float getZpos();
    void setWinSize(int w, int h);
    void pan(float dx, float dy);
private:

    int _w, _h;
    float _angle = 45.0f;
    QVector3D camPos;
    QVector3D whereLook;
    QVector3D whereUp;
};

#endif // CAMERA_H
