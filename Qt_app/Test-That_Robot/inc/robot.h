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

//#include "shaderclass.h"
#include "mesh.h"

struct State{
    float x;
    float y;
    float theta; //angle radians

};

class Robot: public Object
{
public:
    Robot();
    Robot(Mesh *mesh);
    void update(float dt);
    void updateMatrix();

    GLuint ID;

    float vx_r;
    float vy_r;
    float omega; //angular speed
private:

    State state;


    // float L; //distance between wheels
    // float r; //wheel radius
    // float vl, vr; //angular velocities of wheels



};

#endif // ROBOT_H
