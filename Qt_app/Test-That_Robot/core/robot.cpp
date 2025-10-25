#include "robot.h"

Robot::Robot()
{

}

Robot::Robot(Mesh *mesh) : Object(mesh)
{
    isRobot = true;
    _x = 0.0f;
    _y = 0.0f;
    _theta = 0.0f;
    _linearspeed = 0.0f;
    _angularspeed = 0.0f;

    _velocity = QVector3D(0.0f, 0.0f, 0.0f);
}

void Robot::update(float dt)
{
    _velocity = normalizeVector();
    _theta = calculateAngle();
    _x += _linearspeed * qCos(_theta) * dt;
    _y += _linearspeed * qSin(_theta) * dt;
    updateModelMatrixFromPosition();
}

void Robot::start()
{
  _linearspeed = 1.0f; 
}

void Robot::stop()
{
    _linearspeed = 0.0f;
}



float Robot::calculateDistance(float nx, float ny){

    return qSqrt(qPow(nx - _x, 2) + qPow(ny - _y, 2));
}
QVector3D Robot::normalizeVector() {
    float length = qSqrt(qPow(_velocity.x(), 2) + qPow(_velocity.y(), 2));
    if (length > 0) {
        return QVector3D(_velocity.x() / length, _velocity.y() / length, 0.0f);
    }
    return QVector3D(0.0f, 0.0f, 0.0f);
}
float Robot::calculateAngle() { 
    return qAtan2(_velocity.y(), _velocity.x());
}


RobotPos Robot::getRobotPos() const
{
    RobotPos pos;
    pos.x = _x;
    pos.y = _y;
    pos.theta = _theta;
    return pos;
}

