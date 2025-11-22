#ifndef WORLD_H
#define WORLD_H


#include <cerrno>
#include <QMatrix4x4>
#include <memory>
#include <QTimer>
#include <QElapsedTimer>
#include <memory.h>

#include <QJsonObject>
#include <QJsonArray>


#include "gizmohandler.h"
#include "object.h"
#include "robot.h"
#include "obstacle.h"
#include "obstaclecircle.h"
#include "occupancy_grid.h"
#include "scripting/scriptingmanager.h"

struct meshPoints{
    vector<GLfloat> v;
    vector<GLuint> i;
};

class World
{
private:
   std::vector<std::unique_ptr<Object>> _primitives;
    bool _isSelected = false;
    int _selectedObjectIndex = -1;
    size_t _nextObjectId = 1;

    std::unique_ptr<Mesh> _box;
    std::unique_ptr<Mesh> _circle;
    std::unique_ptr<Mesh> _Triangle;
    std::unique_ptr<Mesh> _robotMesh;
    std::unique_ptr<Mesh> _destPoint;

    std::unique_ptr<OccupancyGrid> _grid;
    std::unique_ptr<ScriptingManager> _scriptingManager;
    GizmoHandler * _gizmo;
    meshPoints makeCircleMesh(int nSides);
public:

    

    World();
    ~World() = default;

    void init();
    void setGizmoHandler(GizmoHandler * gizmo);
    void update(float dt);

    //object
    void deleteObject();
    void selectObject(int index);
    void translateObject(float x, float y);
    void scaleObject(float sx, float sy, int handleId);
    void RotateSObject(float Angle);
    Object* getSelectedObject();

    //robot
    void startRobot();
    void stopRobot();
    void setRobotDestination(const QVector3D& destination);
    void startRobotOnPlannedPath();
   // const std::vector<QVector2D>& getPlannedPath() const { return _plannedPath; }

    Object* getObjectById(size_t id);
    //scripts
    std::vector<size_t> getAllRobotIDs();
    Robot* getRobotById(size_t robot_id);

    void runMainScript(const std::string& scriptCode);
    void clearMap();
    void addBoxAt(float x, float y, float w, float h);
    void addRobotAt(float x, float y);
    void addCircleAt(float x, float y, float r);
    void addTriangleAt(float x, float y, float r);
    void updateOccupancyGrid();
    void clearTraces();


    const std::vector<std::unique_ptr<Object>>& getPrimitives() const { return _primitives; }
    int getSelectedObjectIndex() const { return _selectedObjectIndex; }
    const OccupancyGrid* getOccupancyGrid() const { return _grid.get(); }
    bool checkLineOfSight(const QVector2D& p1, const QVector2D& p2) const;


//save
    QJsonObject saveState() const;
    void loadState(const QJsonObject &state);
    void clearAllForLoad();

};

#endif // WORLD_H
