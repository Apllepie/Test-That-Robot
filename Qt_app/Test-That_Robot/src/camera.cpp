#include "camera.h"

Camera::Camera() {

}

void Camera::Init()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    camPos = QVector3D(0.0f, 0.0f, 5.0f);
    whereLook = QVector3D(0.0f, 0.0f, 0.0f);
    whereUp = QVector3D(0.0f, 1.0f, 0.0f);

    view.lookAt(camPos, whereLook, whereUp );
    //projection.perspective(45.0f, float(width())/height(), 0.1f, 100.0f);
    projection.perspective(45.0f, float(1), 0.1f, 100.0f);

}

void Camera::Activate(Shader *shader)
{
    shader->Activate();
    f->glUniformMatrix4fv(f->glGetUniformLocation(shader->ID, "view"),1,GL_FALSE, view.constData());
    f->glUniformMatrix4fv(f->glGetUniformLocation(shader->ID, "projection"),1,GL_FALSE, projection.constData());

}

void Camera::changeProjection(int w, int h, float angle, float start, float end)
{
    const float aspect = float(w) / float((h > 0) ? h : 1);
    projection.setToIdentity();
    projection.perspective(angle, aspect, start, end);
}

void Camera::moveCloser_Away(float delta)
{
    camPos.setZ(camPos.z()+delta * 0.7f);
    view.setToIdentity();
    view.lookAt(camPos, whereLook, whereUp);
}

void Camera::Move(QPoint delta)
{
    float speed = BASE_SPEED * camPos.z();
    camPos.setX(camPos.x() - delta.x() *speed);
    camPos.setY(camPos.y() + delta.y() * speed);
    whereLook.setX(camPos.x() - delta.x() *speed);
    whereLook.setY(camPos.y() + delta.y() * speed);
    view.setToIdentity();
    view.lookAt(camPos, whereLook, whereUp);
}
