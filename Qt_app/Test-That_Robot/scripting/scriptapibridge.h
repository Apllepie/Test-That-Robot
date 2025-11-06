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
    void addBoxAt(float x, float y);

    // get functions
    sol::table getAllRobotIDs(sol::this_state s);
    sol::object getRobotPosition(size_t robot_id, sol::this_state s);
    sol::object getGoalPosition(size_t robot_id, sol::this_state s);

    // robot manipulating
    void setRobotPath(size_t robot_id, sol::table path_table);
};

#endif // SCRIPTAPIBRIDGE_H
