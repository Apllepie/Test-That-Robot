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
    RobotPos getRobotPos() const;
    void setVelocity(const QVector2D& vel) { _velocity = QVector3D(vel,0.0f); }


    float omega; //angular speed

};
#endif // ROBOT_H
