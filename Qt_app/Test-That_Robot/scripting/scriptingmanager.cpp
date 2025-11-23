#include "scriptingmanager.h"
#include "scriptapibridge.h"
#include "core/world.h" // Важно для доступа к методу log
#include <QDebug>

ScriptingManager::ScriptingManager() {
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::io);
}

ScriptingManager::~ScriptingManager() {}

void ScriptingManager::init(World* world) {
    // 1. СОХРАНЯЕМ УКАЗАТЕЛЬ В КЛАССЕ
    _world = world; 

    // 2. Создаем мост
    _apiBridge = std::make_unique<ScriptApiBridge>(world);

    // 3. Перехватываем print. Используем _world (член класса) через [this] или захватываем аргумент world
    // Надежнее захватить 'world' напрямую в лямбду, чтобы не зависеть от 'this' в сложный момент инициализации.
    lua.set_function("print", [world](sol::variadic_args args) {
        if (!world) return; // Защита от краша

        std::string output;
        for (auto arg : args) {
            std::string str = arg.as<std::string>();
            output += str + "\t";
        }
        // Вызываем логгер мира
        world->log(output, World::LogType::LUA);
    });

    // --- Регистрация API Scene и Robot ---
    sol::table scene_api = lua.create_named_table("Scene");
    sol::table robot_api = lua.create_named_table("Robot");

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

    qDebug() << "API registered successfully.";
}

void ScriptingManager::runScriptFromEditor(const std::string& scriptCode) {
    try {
        // Выполняем скрипт
        lua.script(scriptCode);
        
        // Логируем успех (ВАЖНАЯ ПРОВЕРКА НА _world)
        if (_world) {
            _world->log("Script parsed successfully.", World::LogType::INFO);
        }

        // Ищем и вызываем main()
        sol::function main_func = lua["main"];
        if (main_func.valid()) {
            sol::protected_function_result result = main_func();
            if (!result.valid()) {
                sol::error err = result;
                std::string errorMsg = "Runtime Error: " + std::string(err.what());
                qWarning() << errorMsg.c_str();
                
                // Логируем ошибку выполнения
                if (_world) _world->log(errorMsg, World::LogType::ERROR);
            }
        }
    } catch (const sol::error& e) {
        std::string syntaxError = "Syntax Error: " + std::string(e.what());
        qWarning() << syntaxError.c_str();
        
        // Логируем синтаксическую ошибку
        if (_world) _world->log(syntaxError, World::LogType::ERROR);
    } catch (...) {
        // Ловим любые другие краши C++
        if (_world) _world->log("Unknown C++ Exception during script execution.", World::LogType::ERROR);
    }
}