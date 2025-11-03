#include "robot.h"

const float MOVE_SPEED =2.0f; // speed of the robot in units per second
const float STOPPING_DISTANCE =0.01f; //distance to destination to consider "arrived"
const float BRAKING_DISTANCE =0.5f;

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
    if (!_hasDestination || !_grid) {
        return;
    }

    QVector2D directionVector(_destination.x() - _x, _destination.y() - _y);
    float distanceToTarget = directionVector.length();

    // Если мы уже очень близко, просто "телепортируемся" в цель и останавливаемся
    if (distanceToTarget < STOPPING_DISTANCE) {
        _x = _destination.x();
        _y = _destination.y();
        stop();
        updateModelMatrixFromParameters();
        return;
    }

    float currentSpeed;

    // Логика с "тормозной зоной"
    if (distanceToTarget < BRAKING_DISTANCE) {
        // Мы в тормозной зоне. Скорость линейно падает.
        currentSpeed = MOVE_SPEED * (distanceToTarget / BRAKING_DISTANCE);
        // Добавим минимальную скорость, чтобы робот не "завяз"
        currentSpeed = qMax(currentSpeed, 0.2f); // Можно сделать минимальную скорость чуть выше
    } else {
        // Мы далеко от цели, едем на полной скорости.
        currentSpeed = MOVE_SPEED;
    }

    directionVector.normalize();

    // --- ИСПРАВЛЕНИЕ ЗДЕСЬ ---
    // Используем новую, рассчитанную скорость currentSpeed!
    QVector2D moveStep = directionVector * currentSpeed * dt;

    QVector2D nextPos(_x + moveStep.x(), _y + moveStep.y());

    if (_grid->isOccupied(nextPos)) {
        stop();
        return;
    }

    _x = nextPos.x();
    _y = nextPos.y();
    _angle = qRadiansToDegrees(qAtan2(directionVector.y(), directionVector.x())) - 90.0f;

    updateModelMatrixFromParameters();
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

void Robot::setGrid(const OccupancyGrid *grid)
{
    _grid = grid;
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


