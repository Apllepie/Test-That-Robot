#include "scene.h"

Scene::Scene() {}

Scene::~Scene()
{
    for (auto it : primitives){
        it.Destruct();
    }
}

void Scene::initialize()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    primitives.emplace_back(Object(Object::type::BOX, {1.0f, 1.0f, 1.0f}));
    qDebug() <<"objects = " <<primitives.size()<<"\n";
    primitives[0].initialize(":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");
    primitives[0].printVertices();



}

void Scene::resize(int w, int h)
{
    f->glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
    //camera.changeProjection(w, h, 45.0f, 0.1f, 100.0f);
}

void Scene::paint()
{
     f->glClear(GL_COLOR_BUFFER_BIT);

    for (auto& obj : primitives) {
        //qDebug() << "draw\n";
        obj.Draw();
    }
}
