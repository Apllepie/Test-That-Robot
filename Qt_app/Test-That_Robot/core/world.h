#ifndef WORLD_H
#define WORLD_H


#include <cerrno>
#include <QMatrix4x4>
#include <memory>
#include <QTimer>
#include <QElapsedTimer>
#include <memory.h>


#include "object.h"
#include "robot.h"
#include "obstacle.h"
#include "occupancy_grid.h"



class World
{
private:
   std::vector<std::unique_ptr<Object>> _primitives;
    bool _isSelected = false;
    int _selectedObjectIndex = -1;

    std::unique_ptr<Mesh> _box;
    std::unique_ptr<Mesh> _robotMesh;
    std::unique_ptr<Mesh> _destPoint;


public:

    

    World();
    ~World() = default;

    void init();
    void update(float dt);

    //object
    void addRobot();
    void addBox();
    void deleteObject();
    void selectObject(int index);
    void translateObject(float x, float y);

    //robot
    void startRobot();
    void stopRobot();
    void setRobotDestination(const QVector3D& destination);


    const std::vector<std::unique_ptr<Object>>& getPrimitives() const { return _primitives; }
    int getSelectedObjectIndex() const { return _selectedObjectIndex; }




};

#endif // WORLD_H
