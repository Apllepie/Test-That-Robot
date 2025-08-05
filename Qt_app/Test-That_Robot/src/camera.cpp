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
    f->glUniformMatrix4fv(f->glGetUniformLocation(shader->ID, "view"),1,GL_FALSE, view.constData());
    f->glUniformMatrix4fv(f->glGetUniformLocation(shader->ID, "projection"),1,GL_FALSE, projection.constData());
}

void Camera::changeProjection(int w, int h, float angle, float start, float end)
{
    projection.perspective(angle, float(w/h), start, end);
}

void Camera::moveCloser_Away(float delta)
{
    camPos.setZ(camPos.z()+delta);
    view.setToIdentity();
    view.lookAt(camPos, whereLook, whereUp);
}
