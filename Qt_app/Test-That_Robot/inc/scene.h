#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>

#include "object.h"


class Scene
{
public:
    Scene();
    ~Scene();
    void initialize();
    void resize(int w, int h);
    void paint();
    std::vector<Object> primitives;
private:
    QOpenGLExtraFunctions *f;
    Object object;


};

#endif // SCENE_H
