#include "scriptapibridge.h"
#include "core/world.h" // Включаем полный заголовок мира здесь
#include "core/robot.h" // И робота, чтобы получить его данные

ScriptApiBridge::ScriptApiBridge(World* world) : _world(world) {}

// API functions

void ScriptApiBridge::clearMap() {
    _world->clearMap();
}

void ScriptApiBridge::addRobotAt(float x, float y) {
    _world->addRobotAt(x, y);
}

void ScriptApiBridge::addBoxAt(float x, float y) {
    _world->addBoxAt(x, y);
}

sol::table ScriptApiBridge::getAllRobotIDs(sol::this_state s) {
    sol::state_view lua(s);
    sol::table ids = lua.create_table();


    auto robot_ids = _world->getAllRobotIDs();
    for(const auto& id : robot_ids) {
        ids.add(id);
    }
    return ids;
}

sol::object ScriptApiBridge::getRobotPosition(size_t robot_id, sol::this_state s) {
    sol::state_view lua(s);
    Robot* robot = _world->getRobotById(robot_id);
    if (robot) {
        RobotPos pos = robot->getRobotPos();
        return lua.create_table_with("x", pos.x, "y", pos.y);
    }
    return sol::make_object(lua, sol::nil);
}

sol::object ScriptApiBridge::getGoalPosition(size_t robot_id, sol::this_state s) {
    sol::state_view lua(s);
    Robot* robot = _world->getRobotById(robot_id);
    if (robot && robot->hasDestination()) {
        QVector3D dest = robot->getDestination();
        return lua.create_table_with("x", dest.x(), "y", dest.y());
    }
    return sol::make_object(lua, sol::nil);
}

void ScriptApiBridge::setRobotPath(size_t robot_id, sol::table path_table) {
    Robot* robot = _world->getRobotById(robot_id);
    if (!robot) return;

    std::vector<QVector2D> worldPath;
    for (const auto& pair : path_table) {
        sol::table point = pair.second;
        worldPath.emplace_back(point["x"], point["y"]);
    }

    // Вызываем "чистую" функцию робота
    if (!worldPath.empty()) {
        robot->setPath(worldPath);
    }
}
