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

class Scene
{
public:
    Camera camera;

    Mesh *box;
    Mesh *robot_mesh;
    Robot robot;

    std::vector<std::unique_ptr<Object>> primitives;

    std::unique_ptr<Shader> defaultShader;
    std::unique_ptr<Shader> frameShader;

    PickingObject picking;
    int selectedObjectIndex = -1;


    Scene();
    ~Scene();
    void initialize();
    void resize(int w, int h);
    void paint();
    void update(float dt);
    void paintPicking(Camera& camera);
    void selectObject(int index);
    void translateObject(float x, float y, Camera &camera);
    void addBox();
    void deleteObject();
    void startRobot(int key);
    void stopRobot();
    QVector2D getWindowSize();


private:
    int w, h;

    QOpenGLExtraFunctions *f;
    Object object;


    //grid rendering
    // Occupancy Grid
    OcupancyGrid occupancyGrid;

    //functions
};

#endif // SCENE_H
