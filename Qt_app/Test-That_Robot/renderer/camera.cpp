#include "renderer/camera.h"

Camera::Camera() {

}

void Camera::Init()
{
    initializeOpenGLFunctions();

    camPos = QVector3D(0.0f, 0.0f, 5.0f);
    whereLook = QVector3D(0.0f, 0.0f, 0.0f);
    whereUp = QVector3D(0.0f, 1.0f, 0.0f);

    view.lookAt(camPos, whereLook, whereUp );
    
    projection.setToIdentity();

}

void Camera::Activate(Shader *shader)
{
    shader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"),1,GL_FALSE, view.constData());
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"),1,GL_FALSE, projection.constData());

}

void Camera::changeProjection(int w, int h, float angle, float start, float end)
{
    _angle = angle;
    setWinSize(w, h);
    const float aspect = float(w) / float((h > 0) ? h : 1);
    projection.setToIdentity();
    projection.perspective(angle, aspect, start, end);
}

void Camera::moveCloser_Away(float delta)
{
        qDebug() << "Camera move ->>: " << _w << " " << _h;
    float newZ = camPos.z() + delta * camPos.z() * ZOOM_SPEED;

    if (newZ < 0.1f) {
        newZ = 0.1f;
    }

    camPos.setZ(newZ);

    // Обновляем матрицу вида с новым положением камеры
    view.setToIdentity();
    view.lookAt(camPos, whereLook, whereUp);
}

void Camera::Move(QPoint delta)
{

    if(_w == 0 || _h ==0) return;


    float worldUnitsPerPixel = 2.0f * camPos.z() * tanf(qDegreesToRadians(_angle) / 2.0f) / float(_h);

    float dx = delta.x() * worldUnitsPerPixel;
    float dy = delta.y() * worldUnitsPerPixel;

   camPos.setX(camPos.x() - dx);
    camPos.setY(camPos.y() + dy);

    // whereLook needs to move as well to maintain direction
    whereLook.setX(whereLook.x() - dx);
    whereLook.setY(whereLook.y() + dy);

    view.setToIdentity();
    view.lookAt(camPos, whereLook, whereUp);
}

float Camera::getZpos()
{
    return camPos.z();
}

void Camera::setWinSize(int w, int h)
{
    _w = w;
    _h = h;
}

void Camera::pan(float dx, float dy)
{
    camPos.setX(camPos.x() + dx);
    camPos.setY(camPos.y() + dy);

    whereLook.setX(whereLook.x() + dx);
    whereLook.setY(whereLook.y() + dy);

    view.setToIdentity();
    view.lookAt(camPos, whereLook, whereUp);
}