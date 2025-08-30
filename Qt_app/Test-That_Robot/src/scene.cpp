#include "scene.h"

Scene::Scene() {}

Scene::~Scene()
{

    if (defaultShader) {
        defaultShader->Delete();
        defaultShader.reset();
    }
    if (frameShader) {
        frameShader->Delete();
        frameShader.reset();
    }
}

void Scene::initialize()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    camera.Init();
    world.Init(f);

    defaultShader =  std::make_unique<Shader>(":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");
    frameShader = std::make_unique<Shader>(":/Shaders/shaders/pick.vert",":/Shaders/shaders/pick.frag");

}

void Scene::resize(int w, int h)
{
    this->w = w;
    this->h = h;
    world.initPicking(w,h);
    f->glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
    //camera.changeProjection(w, h, 45.0f, 0.1f, 100.0f);
}

void Scene::paint()
{

    camera.Activate(frameShader.get());
    world.paintPicking(frameShader.get());

    camera.Activate(defaultShader.get());
    world.Draw(defaultShader.get(), camera);

}

void Scene::update(float dt)
{
    world.Update(dt);
}

QVector2D Scene::getWindowSize()
{
    return QVector2D(w, h);
}




