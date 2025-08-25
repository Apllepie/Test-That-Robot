#include "robot.h"

Robot::Robot()
{

}

Robot::Robot(Mesh *mesh) : Object(mesh)
{

    state.x = 0.0f;
    state.y = 0.0f;
    state.theta = 0.0f;

    speed = 1.0f;
    speed_X = 0.0;
    speed_Y = 0.0;
    velocity = QVector2D(0.0f, 0.0f);
    omega = 0.0f;

}

void Robot::update(float dt)
{
    QVector3D translation = transMatrix.column(3).toVector3D();
    state.x = translation.x();
    state.y = translation.y();

    velocity = QVector2D(speed_X, speed_Y);

    if (!velocity.isNull()) {
        velocity = velocity.normalized() * 0.5f;
    }

    float dx = velocity.x() * qCos(state.theta) - velocity.y() * qSin(state.theta);
    float dy = velocity.x() * qSin(state.theta) + velocity.y() * qCos(state.theta);

    state.x += dx * dt;
    state.y += dy * dt;
    state.theta += omega * dt;
    updateMatrix(dt, dy, omega);
}

void Robot::start(int key)
{
    switch (key) {
    case Qt::Key_W:
        speed_Y == 0.0f ? speed_Y = 1.0f : speed_Y = 0.0f;
        break;
    case Qt::Key_A:
        speed_X == 0.0f ? speed_X = -1.0f : speed_X = 0.0f;
        break;
    case Qt::Key_S:
        speed_Y == 0.0f ? speed_Y = -1.0f : speed_Y = 0.0f;
        break;
    case Qt::Key_D:
        speed_X == 0.0f ? speed_X = 1.0f : speed_X = 0.0f;
        break;
    default:
        break;
    }

}

void Robot::stop()
{
    speed_X =0.0f;
    speed_Y =0.0f;
}

void Robot::updateMatrix(float dx, float dy, float omega)
{


    XrotateMatrix.setToIdentity();
    YrotateMatrix.setToIdentity();
    ZrotateMatrix.setToIdentity();

    QVector3D translation = transMatrix.column(3).toVector3D();
    if(state.x != translation.x() or state.y != translation.y()){
        transMatrix.setToIdentity();
        transMatrix.translate(state.x, state.y, 0.0f);
    }
    //transMatrix.translate(dx, dy);

    ZrotateMatrix.rotate(qRadiansToDegrees(state.theta), 0.0f, 0.0f, 1.0f);

    modelMatrix = updateModelMatrix();
}

