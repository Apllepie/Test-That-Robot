#ifndef SCRIPTAPIBRIDGE_H
#define SCRIPTAPIBRIDGE_H

#include "sol/sol.hpp"

class World;

class ScriptApiBridge {
private:
    World* _world;

public:

    ScriptApiBridge(World* world);
    ~ScriptApiBridge() = default;

    // APi functions

    // function to manipulate scene
    void clearMap();
    void addRobotAt(float x, float y);
    void addBoxAt(float x, float y, float w, float h);
    void updateOccupancyGrid();

    // get functions
    sol::table getAllRobotIDs(sol::this_state s);
    sol::object getRobotPosition(size_t robot_id, sol::this_state s);
    sol::object getGoalPosition(size_t robot_id, sol::this_state s);

    // robot manipulating
    void setRobotPath(size_t robot_id, sol::table path_table);

    // Occupancy Grid API
    sol::table getGridDimensions(sol::this_state s);
    bool isCellOccupied(int x, int y);
    sol::table worldToGrid(float x, float y, sol::this_state s);
    sol::table gridToWorld(int x, int y, sol::this_state s);

    // geometric API
    sol::table getAllObstacles(sol::this_state s);
    bool checkLineOfSight(float x1, float y1, float x2, float y2);
};

#endif // SCRIPTAPIBRIDGE_H
