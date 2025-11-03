#include "world.h"

//should be divided into smaller parts

// change occupancy grid to dynamic, and aabb

World::World() {

}

void World::init()
{

    _grid = std::make_unique<OccupancyGrid>(40.0f, 40.0f, 0.5f);
    _box = std::make_unique<Mesh>(Mesh(Mesh::type::BOX, {1.0f, 1.0f, 1.0f}));
    _robotMesh = std::make_unique<Mesh>(Mesh( {-0.4f, -0.4f, 0.0f,      1.0f, 0.0f, 1.0f,
                                              -0.4f, 0.4f, 0.0f,             1.0f, 0.0f, 1.0f,
                                              0.4f, 0.4f, 0.0,               1.0f, 0.0f, 1.0f,
                                              0.4f, -0.4f, 0.0,              1.0f, 0.0f, 1.0f,
                                              0.0f, float(0.3*sqrt(3)/3), 0.0f, 1.0f, 1.0f, 0.0f,
                                              0.3f/2.0, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f,
                                              -0.3f/2.0f, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f},{0,1,2,2,0,3,4,5,6 }));
    float s = 0.3f;
    float o = 0.0f;
    float c = 1.0f;
    _destPoint = std::make_unique<Mesh>(Mesh({o, s, o, o, c, o,
                                         s, o, o, o, c, o,
                                         o, -s, o, o, c, o,
                                         -s, o, o, o, c, o,
                                         s/3, s/3, o, o, c, o,
                                        s/3, -s/3, o, o, c, o,
                                        -s/3, -s/3, o, o, c, o,
                                            -s/3, s/3, o, o, c, o,},{0,7,4,7,6,4,4,5,6,5,4,1,5,6,2,6,7,3}) );

    _destPoint->Init();
    _box->Init();
    _robotMesh->Init();
    _primitives.emplace_back(std::make_unique<Object>(_destPoint.get()));
    addRobot();
    addBox();
}


void World::update(float dt)
{
    _grid->updateFromObstacles(_primitives);
    for (size_t i = 0; i < _primitives.size(); ++i) {
        _primitives[i]->update(dt);
        if(_primitives[i]->isRobot){
            Robot* robot = dynamic_cast<Robot*>(_primitives[i].get());
            if(robot->hasDestination()){
                _primitives.front()->Scale(QVector3D(1.0f, 1.0f, 1.0f));
                _primitives.front()->Translate(robot->getDestination());
            }
            else _primitives.front()->Scale(QVector3D(0.0f, 0.0f, 0.0f));
        }
    }
}



void World::addRobot()
{
    auto robot = std::make_unique<Robot>(_robotMesh.get());

    robot->setGrid(_grid.get());

    _primitives.emplace_back(std::move(robot));

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
    for (const auto& obj : _primitives) {
        Robot* robot = dynamic_cast<Robot*>(obj.get());
        if (robot) {
            robot->start();
            break; // one robot for now
        }
    }
}

void World::stopRobot()
{
    for (const auto& obj : _primitives) {
        Robot* robot = dynamic_cast<Robot*>(obj.get());
        if (robot) {
            robot->stop();
            break; // one robot for now
        }
    }
}

void World::setRobotDestination(const QVector3D &destination)
{
    //finding robot
    for (const auto& obj : _primitives) {
        Robot* robot = dynamic_cast<Robot*>(obj.get());
        if (robot) {
            robot->setDestination(destination);
            break; // one robot for now
        }
    }
}




