#include "robot.h"

Robot::Robot()
{

}

Robot::Robot(Mesh *mesh) : Object(mesh)
{

    state.x = 0.0f;
    state.y = 0.0f;
    state.theta = 0.0f;

    vx_r = 0.0f;
    vy_r = 0.0f;
    omega = 0.0f;

}

void Robot::update(float dt)
{
    float dx = vx_r * qCos(state.theta) - vy_r * qSin(state.theta);
    float dy = vx_r * qSin(state.theta) + vy_r * qCos(state.theta);

    state.x += dx * dt;
    state.y += dy * dt;
    state.theta += omega * dt;
    updateMatrix();
}

void Robot::updateMatrix()
{
    transMatrix.setToIdentity();
    transMatrix.translate(state.x,state.y);
    ZrotateMatrix.rotate(qRadiansToDegrees(state.theta), 0.0f, 0.0f, 1.0f);

    modelMatrix = updateModelMatrix();
}

