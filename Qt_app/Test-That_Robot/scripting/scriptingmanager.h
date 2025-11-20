#ifndef SCRIPTINGMANAGER_H
#define SCRIPTINGMANAGER_H

#include "scriptapibridge.h"
#include "sol/sol.hpp"
#include "core/occupancy_grid.h"
#include <string>
#include <vector>

class World;
class ScriptingManager;

class ScriptingManager {
private:
    sol::state lua;

    std::unique_ptr<ScriptApiBridge> _apiBridge;
public:
    ScriptingManager();
    ~ScriptingManager();

    void init(World* world);

    void runScriptFromEditor(const std::string& scriptCode);
};

#endif // SCRIPTINGMANAGER_H
