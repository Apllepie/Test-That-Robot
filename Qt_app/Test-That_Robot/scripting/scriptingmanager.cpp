#include "scriptapibridge.h"
#include "core/world.h"
#include <QDebug>

ScriptingManager::ScriptingManager() {
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::io);
}

ScriptingManager::~ScriptingManager() {}


void ScriptingManager::init(World* world) {
    // made bbridge
    _apiBridge = std::make_unique<ScriptApiBridge>(world);

    // made table Api
    sol::table scene_api = lua.create_named_table("Scene");
    sol::table robot_api = lua.create_named_table("Robot");

    // 3. Регистрируем функции из МОСТА, а не из World!
    scene_api.set_function("clear_map", &ScriptApiBridge::clearMap, _apiBridge.get());
    scene_api.set_function("add_robot", &ScriptApiBridge::addRobotAt, _apiBridge.get());
    scene_api.set_function("add_rec", &ScriptApiBridge::addBoxAt, _apiBridge.get());
    scene_api.set_function("add_circle", &ScriptApiBridge::addCircleAt, _apiBridge.get());
    scene_api.set_function("add_triangle", &ScriptApiBridge::addTriangleAt, _apiBridge.get());
    scene_api.set_function("clear_trace", &ScriptApiBridge::clearTrace, _apiBridge.get());
    scene_api.set_function("rebuild_collision_map", &ScriptApiBridge::updateOccupancyGrid, _apiBridge.get());
   
    scene_api.set_function("get_grid_dimensions", &ScriptApiBridge::getGridDimensions, _apiBridge.get());
    scene_api.set_function("is_cell_occupied", &ScriptApiBridge::isCellOccupied, _apiBridge.get());
    scene_api.set_function("world_to_grid", &ScriptApiBridge::worldToGrid, _apiBridge.get());
    scene_api.set_function("grid_to_world", &ScriptApiBridge::gridToWorld, _apiBridge.get());

    scene_api.set_function("get_all_obstacles", &ScriptApiBridge::getAllObstacles, _apiBridge.get());
    scene_api.set_function("check_line_of_sight", &ScriptApiBridge::checkLineOfSight, _apiBridge.get());
    
    robot_api.set_function("get_all_ids", &ScriptApiBridge::getAllRobotIDs, _apiBridge.get());
    robot_api.set_function("get_pos", &ScriptApiBridge::getRobotPosition, _apiBridge.get());
    robot_api.set_function("get_goal_pos", &ScriptApiBridge::getGoalPosition, _apiBridge.get());
    robot_api.set_function("set_path", &ScriptApiBridge::setRobotPath, _apiBridge.get());

    qDebug() << "C++ API registered in Lua. Global table 'Scene' 'Robot' is now available.";
}


// Эта функция будет вызываться по кнопке "Run"
void ScriptingManager::runScriptFromEditor(const std::string& scriptCode) {
    try {
        // Выполняем скрипт, чтобы определить все функции, включая main
        lua.script(scriptCode);
        qDebug() << "Script from editor executed.";

        // Ищем и вызываем функцию main()
        sol::function main_func = lua["main"];
        if (main_func.valid()) {
            qDebug() << "Executing main() function from script...";
            sol::protected_function_result result = main_func(); // Запускаем!
            if (!result.valid()) {
                sol::error err = result;
                qWarning() << "LUA RUNTIME ERROR in main():" << err.what();
            }
        } else {
            qDebug() << "INFO: No main() function found in the script.";
        }
    } catch (const sol::error& e) {
        qWarning() << "LUA SYNTAX ERROR:" << e.what();
    }
}
