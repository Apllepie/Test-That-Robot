#include "world.h"

World::World() {}


void World::Init(QOpenGLExtraFunctions *f)
{
    this->f = f;


    box = std::make_unique<Mesh>(Mesh(Mesh::type::BOX, {1.0f, 1.0f, 1.0f}));
    box->Init();

    robotMesh = std::make_unique<Mesh>(Mesh( {-0.4f, -0.4f, 0.0f,      1.0f, 0.0f, 1.0f,
                           -0.4f, 0.4f, 0.0f,             1.0f, 0.0f, 1.0f,
                           0.4f, 0.4f, 0.0,               1.0f, 0.0f, 1.0f,
                           0.4f, -0.4f, 0.0,              1.0f, 0.0f, 1.0f,
                           0.0f, float(0.3*sqrt(3)/3), 0.0f, 1.0f, 1.0f, 0.0f,
                           0.3f/2.0, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f,
                           -0.3f/2.0f, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f},{0,1,2,2,0,3,4,5,6 }));

    robotMesh->Init();

    primitives.reserve(128);

    primitives.emplace_back(std::make_unique<Robot>(robotMesh.get()));
    primitives.back()->initialize();

    addBox();

    grid.Init(20, 20, 1.0f, 8);

    grid.SetCell(2, 2, 1);
    grid.SetCell(5, 8, 1);
    grid.Update();
}

void World::Update(float dt)
{
    for (size_t i = 0; i < primitives.size(); ++i) {
        primitives[i]->update(dt);
    }
}

void World::Draw(Shader *dShader, Camera &camera)
{
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    grid.Draw(camera);
    grid.DrawGrid(camera);

    for (size_t i = 0; i < primitives.size(); ++i) {
        dShader->Activate();

        bool isSelected = (static_cast<int>(i) == selectedObjectIndex);
        GLint loc = f->glGetUniformLocation(dShader->ID, "isSelected");
        f->glUniform1i(loc, isSelected ? 1 : 0);
        primitives[i]->update(0.016f);
        primitives[i]->Draw(dShader);
    }

}



void World::initPicking(int w, int h)
{
    picking.init(w,h);
}

void World::paintPicking(Shader *fShader)
{
    picking.enableWrite();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint id =0;

    for(auto& obj :primitives){
        fShader->Activate();

        GLint loc = f->glGetUniformLocation(fShader->ID, "objectID");
        GLint mvp = f->glGetUniformLocation(fShader->ID, "model");
        f->glUniform1ui(loc, id+1);
        f->glUniformMatrix4fv(mvp,1, GL_FALSE, obj->modelMatrix.constData());


        obj->Draw(fShader);
        id++;
    }
    picking.disableWrite();
}

void World::addBox()
{
    primitives.emplace_back(std::make_unique<Object>(box.get()));
    primitives.back()->initialize();

}

void World::deleteObject()
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

void World::selectObject(int index)
{
    if (index >= 0 && index < static_cast<int>(primitives.size())) {
        selectedObjectIndex = index;
    } else {
        selectedObjectIndex = -1;
    }
    qDebug() << "selected " << selectedObjectIndex;
}

void World::translateObject(float x, float y, Camera & camera){

    if(selectedObjectIndex == -1) return;

    float speed = camera.getZpos() * 0.0011f;
    primitives[selectedObjectIndex]->Translate(x*speed, y*speed, 0.0f);

}

void World::startRobot(int key)
{
    primitives.front()->start(key);
    qDebug() << "robot START \n";
}

void World::stopRobot()
{
    primitives.front()->stop();
    qDebug() << "robot STOP \n";
}
