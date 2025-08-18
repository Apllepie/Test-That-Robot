#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>

#include "camera.h"
#include "shaderclass.h"
#include "VAO.h"
#include "object.h"
#include "pickingobject.h"


class Scene
{
public:
    Scene();
    ~Scene();
    void initialize();
    void resize(int w, int h);
    void paint(Camera& camera);
    void paintPicking(Camera& camera);
    void selectObject(int index);
    void translateObject(float x, float y, Camera &camera);
    void addBox();
    void deleteObject();
    

    std::vector<Object> primitives;

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
    VAO *gridVAO;
    VBO *gridVBO;
    GLsizei gridVertexCount = 0;

    VAO *axisVAO;
    VBO *axisVBO;
    GLsizei axisVertexCount = 0;


    //functions
    void initGrid();
};

#endif // SCENE_H
