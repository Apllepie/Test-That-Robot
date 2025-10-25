#ifndef ROBOT_H
#define ROBOT_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>
#include <object.h>
#include <qmath.h>
#include <QKeyEvent>

//#include "shaderclass.h"
#include "mesh.h"

struct RobotPos{
    float x;
    float y;
    float theta; //angle radians

};

class Robot: public Object
{
public:
    Robot();
    Robot(Mesh *mesh);

    void update(float dt) override;
    void start(int key) override;
    void stop() override;
    void updateMatrix(float dx, float dy, float omega);

    GLuint ID;

    float speed_X;
    float speed_Y;
    float speed;
    QVector2D velocity;
    float omega; //angular speed
private:

    RobotPos state;


    // float L; //distance between wheels
    // float r; //wheel radius
    // float vl, vr; //angular velocities of wheels



};

#endif // ROBOT_H
