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

void ScriptApiBridge::addBoxAt(float x, float y, float w, float h) {
    _world->addBoxAt(x, y, w, h);
}
void ScriptApiBridge::updateOccupancyGrid() {
    _world->updateOccupancyGrid();
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

sol::table ScriptApiBridge::getGridDimensions(sol::this_state s) {
    sol::state_view lua(s);
    sol::table dims = lua.create_table();
    if (const OccupancyGrid* grid = _world->getOccupancyGrid()) {
        dims["width"] = grid->getWidth();
        dims["height"] = grid->getHeight();
    }
    return dims;
}

bool ScriptApiBridge::isCellOccupied(int x, int y) {
    if (const OccupancyGrid* grid = _world->getOccupancyGrid()) {
        return grid->isOccupied(x, y);
    }
    return true; // Считаем занятым, если сетки нет
}

sol::table ScriptApiBridge::worldToGrid(float x, float y, sol::this_state s) {
    sol::state_view lua(s);
    sol::table gridPos = lua.create_table();
    if (const OccupancyGrid* grid = _world->getOccupancyGrid()) {
        int gridX, gridY;
        grid->worldToGrid(QVector2D(x, y), gridX, gridY);
        gridPos["x"] = gridX;
        gridPos["y"] = gridY;
    }
    return gridPos;
}

sol::table ScriptApiBridge::gridToWorld(int x, int y, sol::this_state s) {
    sol::state_view lua(s);
    sol::table worldPos = lua.create_table();
    if (const OccupancyGrid* grid = _world->getOccupancyGrid()) {
        QVector2D pos = grid->gridToWorld(x, y);
        worldPos["x"] = pos.x();
        worldPos["y"] = pos.y();
    }
    return worldPos;
}


// Геометрическое API
sol::table ScriptApiBridge::getAllObstacles(sol::this_state s) {
    sol::state_view lua(s);
    sol::table obstacles = lua.create_table();
    for (const auto& obj : _world->getPrimitives()) {
        if (const Obstacle* obstacle = dynamic_cast<const Obstacle*>(obj.get())) {
            sol::table obs_data = lua.create_table();
            obs_data["id"] = obstacle->getId();
            obs_data["x"] = obstacle->getX();
            obs_data["y"] = obstacle->getY();

            sol::table vertices = lua.create_table();
            for (const auto& v : obstacle->getGlobalVertices()) {
                vertices.add(lua.create_table_with("x", v.x(), "y", v.y()));
            }
            obs_data["vertices"] = vertices;
            obstacles.add(obs_data);
        }
    }
    return obstacles;
}

bool ScriptApiBridge::checkLineOfSight(float x1, float y1, float x2, float y2) {
    return _world->checkLineOfSight(QVector2D(x1, y1), QVector2D(x2, y2));
}
