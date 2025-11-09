#include "world.h"

//should be divided into smaller parts

// change occupancy grid to dynamic, and aabb

World::World() {

}

void World::init()
{
    //run scripts
    _scriptingManager = std::make_unique<ScriptingManager>();
    _scriptingManager->init(this);

    _grid = std::make_unique<OccupancyGrid>(40.0f, 40.0f, 0.25f);
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
    addRobotAt(0, 0);
    addBoxAt(0, 0);
}


void World::update(float dt)
{
    _grid->updateFromObstacles(_primitives);

    // 1. Обновляем логику всех объектов (этот цикл остается)
    for (const auto& obj : _primitives) {
        obj->update(dt);
    }

    // 2. УПРАВЛЯЕМ ВИДИМОСТЬЮ МАРКЕРОВ
    // Пройдемся по всем роботам на сцене
    for (const auto& obj : _primitives)
    {
        // Нас интересуют только объекты-роботы
        if (auto robot = dynamic_cast<Robot*>(obj.get()))
        {
            // Получаем ID маркера, который "привязан" к этому роботу
            size_t marker_id = robot->getDestinationMarkerId();
            if (marker_id == 0) continue; // Если у робота нет маркера, пропускаем

            // Находим сам объект-маркер по его ID
            Object* marker = getObjectById(marker_id);
            if (!marker) continue; // Если по какой-то причине маркер не найден, пропускаем

            // Теперь для этого конкретного маркера принимаем одно из двух решений:
            if (robot->hasDestination()) {
                // РЕШЕНИЕ А: У робота ЕСТЬ цель.
                // Делаем маркер видимым и перемещаем его в точку назначения.
                marker->Scale(QVector3D(1.0f, 1.0f, 1.0f));
                marker->Translate(robot->getDestination());
            } else {
                // РЕШЕНИЕ Б: У робота НЕТ цели.
                // Делаем маркер невидимым.
                marker->Scale(QVector3D(0.0f, 0.0f, 0.0f));
            }
        }
    }
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
{   // Проверяем, выбран ли какой-то объект
    if (_selectedObjectIndex == -1) {
        qWarning() << "Right-clicked, but no object selected.";
        return;
    }

    // Пытаемся преобразовать выбранный объект в робота
    Robot* selected_robot = dynamic_cast<Robot*>(_primitives[_selectedObjectIndex].get());

    if (selected_robot) {
        // Если это робот, устанавливаем ему цель
        selected_robot->setDestination(destination);
        qDebug() << "Destination set for selected robot ID" << selected_robot->getId() << "to" << destination;
    } else {
        qWarning() << "Right-clicked, but the selected object is not a robot.";
    }
}

std::vector<size_t> World::getAllRobotIDs()
{
    std::vector<size_t> robot_ids;
    for (const auto& obj_ptr : _primitives)
    {
        if (dynamic_cast<Robot*>(obj_ptr.get()))
        {
            robot_ids.push_back(obj_ptr->getId());
        }
    }

    return robot_ids;
}
Object* World::getObjectById(size_t id)
{
    for (const auto& obj_ptr : _primitives) {
        if (obj_ptr->getId() == id) {
            return obj_ptr.get();
        }
    }
    return nullptr;
}

Robot *World::getRobotById(size_t robot_id)
{
    for (const auto& obj_ptr : _primitives) {
        if (obj_ptr->getId() == robot_id) {
            return dynamic_cast<Robot*>(obj_ptr.get());
        }
    }
    return nullptr;
}


//Scripts
void World::runMainScript(const std::string &scriptCode)
{
    _scriptingManager->runScriptFromEditor(scriptCode);

}

void World::updateOccupancyGrid()
{
    if (_grid) {
        _grid->updateFromObstacles(_primitives);
    }
}


void World::clearMap()
{
    if (_primitives.empty()) {
        return;
    }

    std::vector<std::unique_ptr<Object>> objectsToKeep;

    for (auto& obj : _primitives) {
        // Оставляем объект, только если он НЕ робот, НЕ препятствие И НЕ маркер цели.
        if (!dynamic_cast<Robot*>(obj.get()) &&
            !dynamic_cast<Obstacle*>(obj.get()) &&
            obj->getMesh() != _destPoint.get())
        {
            objectsToKeep.push_back(std::move(obj));
        }
    }

    _primitives.swap(objectsToKeep);
    _selectedObjectIndex = -1;
    qDebug() << "Map cleared.";
}

void World::addBoxAt(float x, float y)
{
    auto box = std::make_unique<Obstacle>(_box.get(), 1.0f, 1.0f);
    box->Translate(QVector3D(x, y, 0));
    _primitives.emplace_back(std::move(box));
}

void World::addRobotAt(float x, float y)
{
    auto robot = std::make_unique<Robot>(_robotMesh.get());
    robot->Translate(QVector3D(x, y, 0));
    robot->setGrid(_grid.get());
    robot->setId(_nextObjectId++); // Используем общий счетчик ID

    //
    auto dest_marker = std::make_unique<Object>(_destPoint.get());
    dest_marker->setId(_nextObjectId++); // Даем маркеру свой уникальный ID

    //
    robot->setDestinationMarkerId(dest_marker->getId());

    //
    _primitives.emplace_back(std::move(robot));
    _primitives.emplace_back(std::move(dest_marker));
}
bool World::checkLineOfSight(const QVector2D& p1, const QVector2D& p2) const
{
    for (const auto& obj : _primitives) {
        if (const Obstacle* obstacle = dynamic_cast<const Obstacle*>(obj.get())) {
            // AABB-тест для быстрой отбраковки
            QVector3D pos = obj->getModelMatrix().column(3).toVector3D();
            QVector3D scale = obj->getScale(); // Получаем масштаб из объекта
            float halfWidth = scale.x() / 2.0f;
            float halfHeight = scale.y() / 2.0f;
            
            // Простая проверка пересечения линии с AABB препятствия
            // (Это не совсем точно для повернутых объектов, но для начала сойдет)
            QRectF obstacleBounds(pos.x() - halfWidth, pos.y() - halfHeight, scale.x(), scale.y());
            if (obstacleBounds.intersects(QRectF(p1.toPointF(), p2.toPointF()))) {
                // Более точная проверка нужна здесь, но пока будем считать, что пересечение есть
                return false; // Нет прямой видимости
            }
        }
    }
    return true; // Прямая видимость есть
}


//Save and load
void World::clearAllForLoad()
{
    _primitives.clear();
    _selectedObjectIndex = -1;
    _nextObjectId = 1; // Сбрасываем счетчик ID
}

QJsonObject World::saveState() const
{
    QJsonObject worldState;
    QJsonArray objectsArray;

    for (const auto& obj : _primitives) {
        // Пропускаем маркеры назначения, так как они создаются динамически
        if (obj->getMesh() == _destPoint.get()) {
            continue;
        }

        QJsonObject objJson;
        objJson["type"] = obj->getType();
        objJson["x"] = obj->getX();
        objJson["y"] = obj->getY();

        // Если это препятствие, сохраним его размеры
        if (const Obstacle* obstacle = dynamic_cast<const Obstacle*>(obj.get())) {
            objJson["width"] = obstacle->getWidth();
            objJson["height"] = obstacle->getHeight();
        }

        objectsArray.append(objJson);
    }

    worldState["objects"] = objectsArray;
    return worldState;
}

void World::loadState(const QJsonObject &state)
{
    clearAllForLoad(); // Полностью очищаем мир

    if (state.contains("objects") && state["objects"].isArray()) {
        QJsonArray objectsArray = state["objects"].toArray();

        for (const QJsonValue &objValue : objectsArray) {
            QJsonObject objJson = objValue.toObject();

            QString type = objJson["type"].toString();
            float x = objJson["x"].toDouble();
            float y = objJson["y"].toDouble();

            if (type == "robot") {
                addRobotAt(x, y);
            } else if (type == "obstacle") {
                // Восстанавливаем препятствие с его размерами
                float width = objJson.contains("width") ? objJson["width"].toDouble() : 1.0f;
                float height = objJson.contains("height") ? objJson["height"].toDouble() : 1.0f;
                
                auto box = std::make_unique<Obstacle>(_box.get(), width, height);
                box->Translate(QVector3D(x, y, 0));
                _primitives.emplace_back(std::move(box));
            }
        }
    }
}


