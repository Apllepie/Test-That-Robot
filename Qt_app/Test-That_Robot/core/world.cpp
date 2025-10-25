#include "world.h"

//should be divided into smaller parts

// change occupancy grid to dynamic, and aabb

World::World() {

}

void World::init()
{
    _box = std::make_unique<Mesh>(Mesh(Mesh::type::BOX, {1.0f, 1.0f, 1.0f}));
    _robotMesh = std::make_unique<Mesh>(Mesh( {-0.4f, -0.4f, 0.0f,      1.0f, 0.0f, 1.0f,
                                              -0.4f, 0.4f, 0.0f,             1.0f, 0.0f, 1.0f,
                                              0.4f, 0.4f, 0.0,               1.0f, 0.0f, 1.0f,
                                              0.4f, -0.4f, 0.0,              1.0f, 0.0f, 1.0f,
                                              0.0f, float(0.3*sqrt(3)/3), 0.0f, 1.0f, 1.0f, 0.0f,
                                              0.3f/2.0, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f,
                                              -0.3f/2.0f, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f},{0,1,2,2,0,3,4,5,6 }));

    _box->Init();
    _robotMesh->Init();
    addRobot();
    addBox();
}


void World::update(float dt)
{
    for (size_t i = 0; i < _primitives.size(); ++i) {
        _primitives[i]->update(dt);
    }
}



void World::addRobot()
{
    _primitives.emplace_back(std::make_unique<Robot>(_robotMesh.get()));

}

void World::addBox()
{
    _primitives.emplace_back(std::make_unique<Obstacle>(_box.get(),1.0f,1.0f));

}

void World::deleteObject()
{
    if(_selectedObjectIndex == -1) return;

    if(_primitives.size() == 1){
        _primitives.clear();
    } else {
        _primitives.erase(_primitives.begin() + _selectedObjectIndex);
    }
    _selectedObjectIndex = -1;
    
}

void World::selectObject(int index)
{
    if (index >= 0 && index < static_cast<int>(_primitives.size())) {
        _selectedObjectIndex = index;
    } else {
        _selectedObjectIndex = -1;
    }
    qDebug() << "selected " << _selectedObjectIndex;
}

void World::translateObject(float x, float y){
    if(_selectedObjectIndex == -1) return;
    _primitives[_selectedObjectIndex]->Translate(x, y, 0.0f);
}

void World::startRobot()
{
    _primitives.front()->start();
    qDebug() << "robot START \n";
}

void World::stopRobot()
{
    _primitives.front()->stop();
    qDebug() << "robot STOP \n";
}
