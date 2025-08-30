#ifndef WORLD_H
#define WORLD_H


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



class World
{
public:
    std::unique_ptr<Mesh> box;
    std::unique_ptr<Mesh> robotMesh;


    std::vector<std::unique_ptr<Object>> primitives;

    PickingObject picking;
    int selectedObjectIndex = -1;

    OcupancyGrid grid;

    World();

    void Init(QOpenGLExtraFunctions *f);
    void Update(float dt);
    void Draw(Shader *dShader, Camera &camera);

    void initPicking(int w, int h);
    void paintPicking(Shader *fShader);

    void addBox();
    void deleteObject();
    void selectObject(int index);
    void translateObject(float x, float y, Camera &camera);

    void startRobot(int key);
    void stopRobot();

private:
    QOpenGLExtraFunctions *f;

    bool isSelected = false;

};

#endif // WORLD_H
