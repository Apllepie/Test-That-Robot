#ifndef CAMERA_H
#define CAMERA_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <qmatrix4x4.h>
#include "shaderclass.h"


class Camera
{
public:
    Camera();

    void Init();
    void Activate(Shader *shader);
    void changeProjection(int w, int h, float angle, float start, float end);
    void moveCloser_Away(float delta);
private:
    QOpenGLExtraFunctions *f;
    QMatrix4x4 view;
    QMatrix4x4 projection;

    QVector3D camPos;
    QVector3D whereLook;
    QVector3D whereUp;
};

#endif // CAMERA_H
