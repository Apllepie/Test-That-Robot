#include "scene.h"

Scene::Scene() {}

Scene::~Scene()
{
    primitives.clear();

    defaultShader->Delete();
    frameShader->Delete();

    delete box;
    delete robot_mesh;
}

void Scene::initialize()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();
    camera.Init();

    defaultShader =  std::make_unique<Shader>(":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");
    frameShader = std::make_unique<Shader>(":/Shaders/shaders/pick.vert",":/Shaders/shaders/pick.frag");

    box = new Mesh(Mesh::type::BOX, {1.0f, 1.0f, 1.0f});
    box->Init();
     robot_mesh = new Mesh( {-0.4f, -0.4f, 0.0f,      1.0f, 0.0f, 1.0f,
                  -0.4f, 0.4f, 0.0f,             1.0f, 0.0f, 1.0f,
                   0.4f, 0.4f, 0.0,               1.0f, 0.0f, 1.0f,
                   0.4f, -0.4f, 0.0,              1.0f, 0.0f, 1.0f,
                     0.0f, float(0.3*sqrt(3)/3), 0.0f, 1.0f, 1.0f, 0.0f,
                     0.3f/2.0, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f,
                        -0.3f/2.0f, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f},{0,1,2,2,0,3,4,5,6 });

    robot_mesh->Init();

   // robot = Robot(robot_mesh);
   // robot.initialize();
    primitives.reserve(128);
    primitives.emplace_back(make_unique<Robot>(robot_mesh));
    primitives.back()->initialize();
    if (dynamic_cast<Robot*>(primitives.front().get())){

        qDebug() << "Это Robot\n";}
    addBox();

    occupancyGrid.Init(20, 20, 1.0f, 8);

    occupancyGrid.SetCell(2, 2, 1);
    occupancyGrid.SetCell(5, 8, 1);
    occupancyGrid.Update();
}

void Scene::resize(int w, int h)
{
    this->w = w;
    this->h = h;
    picking.init(w,h);
    f->glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
    //camera.changeProjection(w, h, 45.0f, 0.1f, 100.0f);
}

void Scene::paint()
{
    camera.Activate(defaultShader.get());

    //primitives.front().update(1.0f);
    picking.enableWrite();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintPicking(camera);
    picking.disableWrite();

    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    occupancyGrid.Draw(camera);
    occupancyGrid.DrawGrid(camera);

    for (size_t i = 0; i < primitives.size(); ++i) {
        defaultShader->Activate();
        camera.Activate(defaultShader.get());

        bool isSelected = (static_cast<int>(i) == selectedObjectIndex);
        GLint loc = f->glGetUniformLocation(defaultShader->ID, "isSelected");
        f->glUniform1i(loc, isSelected ? 1 : 0);
        primitives[i]->update(0.016f);
        primitives[i]->Draw(defaultShader.get());
    }


}

void Scene::update(float dt)
{
    for (size_t i = 0; i < primitives.size(); ++i) {
        primitives[i]->update(dt);
    }
}

void Scene::paintPicking(Camera &camera)
{
    GLuint id =0;

    for(auto& obj :primitives){
        frameShader->Activate();
        camera.Activate(frameShader.get());

        GLint loc = f->glGetUniformLocation(frameShader->ID, "objectID");
        GLint mvp = f->glGetUniformLocation(frameShader->ID, "model");
        f->glUniform1ui(loc, id+1);
        f->glUniformMatrix4fv(mvp,1, GL_FALSE, obj->modelMatrix.constData());


        obj->Draw(frameShader.get());
        id++;
    }
}

void Scene::selectObject(int index)
{
    if (index >= 0 && index < static_cast<int>(primitives.size())) {
        selectedObjectIndex = index;
    } else {
        selectedObjectIndex = -1;
    }
    qDebug() << "selected " << selectedObjectIndex;
}

void Scene::translateObject(float x, float y, Camera & camera){

    if(selectedObjectIndex == -1) return;

         float speed = camera.getZpos() * 0.0011f;
         primitives[selectedObjectIndex]->Translate(x*speed, y*speed, 0.0f);

}

void Scene::addBox()
{
    primitives.emplace_back(make_unique<Object>(box));
    primitives.back()->initialize();
}


void Scene::deleteObject()
{
    if(selectedObjectIndex == -1) return;

    if(primitives.size() == 1){
       // primitives.front().clear();

        primitives.clear();
    } else {
        primitives.erase(primitives.begin() + selectedObjectIndex);
    }
    selectedObjectIndex = -1;
}

void Scene::startRobot(int key)
{
    primitives.front()->start(key);
    qDebug() << "robot START \n";
}

void Scene::stopRobot()
{
    primitives.front()->stop();
    qDebug() << "robot STOP \n";
}

QVector2D Scene::getWindowSize()
{
    return QVector2D(w, h);
}




