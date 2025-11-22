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
      // ИСПРАВЛЕНИЕ: Если пути нет, просто ничего не делаем в этот кадр.
    // НЕ вызываем stop(), чтобы не сбросить флаг цели.
    if (_path.empty() || _currentPathIndex < 0 || _currentPathIndex >= (int)_path.size()) {
        return; // Просто выходим
    }

    // --- Весь остальной код остается без изменений ---

    QVector2D currentTarget = _path[_currentPathIndex];
    QVector2D currentPos(_x, _y);
    QVector2D directionVector = currentTarget - currentPos;
    float distanceToTarget = directionVector.length();

    if (distanceToTarget < STOPPING_DISTANCE) {
        _currentPathIndex++;
        
        if (_currentPathIndex >= _path.size()) {
            _x = currentTarget.x();
            _y = currentTarget.y();
            stop(); // Вызываем stop() только когда путь ЗАКОНЧИЛСЯ
            updateModelMatrixFromParameters();
            return;
        }
        currentTarget = _path[_currentPathIndex];
        directionVector = currentTarget - currentPos;
        distanceToTarget = directionVector.length();
    }
    
    float currentSpeed = MOVE_SPEED;
    directionVector.normalize();
    float moveDistance = currentSpeed * dt;

    if (moveDistance >= distanceToTarget) {
        _x = currentTarget.x();
        _y = currentTarget.y();
    } else {
        QVector2D moveStep = directionVector * moveDistance;
        _x += moveStep.x();
        _y += moveStep.y();
    }

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
    _path.clear(); 
    _currentPathIndex = -1;
    _hasDestination = false;
}

void Robot::setPath(const std::vector<QVector2D>& path){
if (path.empty()) {
        stop();
        return;
    }
    _path = path;
    _currentPathIndex = 0; // start from the first point
    _hasDestination = true;
    _linearspeed = MOVE_SPEED; // Start moving
    _destination = QVector3D(path.back().x(), path.back().y(), 0.0f);
    _hasDestination = true;
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


