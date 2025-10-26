#ifndef ROBOT_H
#define ROBOT_H


#include <cerrno>
#include <QMatrix4x4>
#include <object.h>
#include <qmath.h>



struct RobotPos{
    float x;
    float y;
    float theta; //angle radians

};

class Robot: public Object
{

private: 
    float _theta;
    float _linearspeed;
    float _angularspeed;
    QVector3D _velocity;

    QVector3D _destination; // --- НОВОЕ ПОЛЕ: Точка назначения
    bool _hasDestination = false;
    
    //functions
    float calculateDistance(float nx, float ny);
    QVector3D normalizeVector();
    float calculateAngle();

public:
    Robot();
    Robot(Mesh *mesh);

    void update(float dt) override;
    void start() override;
    void stop() override;
    void setDestination(const QVector3D& dest);
    RobotPos getRobotPos() const;
    void setVelocity(const QVector2D& vel) { _velocity = QVector3D(vel,0.0f); }
    bool hasDestination() const { return _hasDestination; }
    QVector3D getDestination() const { return _destination; }


    float omega; //angular speed

};
#endif // ROBOT_H
