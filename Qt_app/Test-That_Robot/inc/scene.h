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

#include "camera.h"
#include "shaderclass.h"
#include "VAO.h"
#include "object.h"
#include "pickingobject.h"
#include "robot.h"


class Scene
{
public:
    Scene();
    ~Scene();
    void initialize();
    void resize(int w, int h);
    void paint(Camera& camera);
    void update(float dt);
    void paintPicking(Camera& camera);
    void selectObject(int index);
    void translateObject(float x, float y, Camera &camera);
    void addBox();
    void deleteObject();
    void startRobot(int key);
    void stopRobot();
    QVector2D getWindowSize();

    Mesh *box;
    Mesh *robot_mesh;
    Robot robot;

    std::vector<std::unique_ptr<Object>> primitives;

    Shader *defaultShader;
    Shader *frameShader;
    PickingObject picking;
    int selectedObjectIndex = -1;


private:
    int w, h;

    QOpenGLExtraFunctions *f;
    Object object;


    //grid rendering
    Shader* gridShader = nullptr;
    VAO gridVAO; //to do new class for this or mesh
    VBO gridVBO;
    GLsizei gridVertexCount = 0;

    VAO axisVAO;
    VBO axisVBO;
    GLsizei axisVertexCount = 0;


    //functions
    void initGrid();
};

#endif // SCENE_H
