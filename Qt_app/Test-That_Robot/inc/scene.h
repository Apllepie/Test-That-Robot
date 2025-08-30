#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>
#include <memory>
#include <QTimer>
#include <QElapsedTimer>
#include <memory.h>

#include "camera.h"
#include "shaderclass.h"
#include "object.h"
#include "pickingobject.h"
#include "robot.h"
#include "ocupancygrid.h"
#include "world.h"

class Scene
{
public:
    Camera camera;
    World world;

    std::unique_ptr<Shader> defaultShader;
    std::unique_ptr<Shader> frameShader;




    Scene();
    ~Scene();
    void initialize();
    void resize(int w, int h);
    void paint();
    void update(float dt);

    QVector2D getWindowSize();


private:
    int w, h;

    QOpenGLExtraFunctions *f;


    //functions
};

#endif // SCENE_H
