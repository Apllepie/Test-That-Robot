#include "robot.h"

const float MOVE_SPEED = 2.0f; // speed of the robot in units per second
const float STOPPING_DISTANCE = 0.1f; //distance to destination to consider "arrived"

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
     if (!_hasDestination) {
        return;
    }

    // vector from current position to destination
    QVector2D directionVector(_destination.x() - _x, _destination.y() - _y);

    // Check distance to destination
    if (directionVector.length() < STOPPING_DISTANCE) {
        stop(); // We have reached the destination
        return;
    }

    // 1. Update orientation (angle) of the robot
    // The angle at which the robot should look to move towards the target
    _angle = qRadiansToDegrees(qAtan2(directionVector.y(), directionVector.x())) - 90.0f;

    // 2. Update position of the robot
    // Normalize the direction vector to get a unit vector
    directionVector.normalize();
    
    _x += directionVector.x() * _linearspeed * dt;
    _y += directionVector.y() * _linearspeed * dt;

    // Update model matrix
    _modelMatrix.setToIdentity();
    _modelMatrix.translate(_x, _y, 0);
    _modelMatrix.rotate(_angle, 0, 0, 1); 
    _modelMatrix.scale(_scale, _scale, _scale);
    // _velocity = normalizeVector();
    // _theta = calculateAngle();
    // _x += _linearspeed * qCos(_theta) * dt;
    // _y += _linearspeed * qSin(_theta) * dt;
    // updateModelMatrixFromPosition();
}

void Robot::start()
{
  _linearspeed = MOVE_SPEED; 
}

void Robot::stop()
{
    _linearspeed = 0.0f;
    _hasDestination = false;
}

void Robot::setDestination(const QVector3D& dest)
{
    _destination = dest;
    _hasDestination = true;
    _linearspeed = MOVE_SPEED;
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


