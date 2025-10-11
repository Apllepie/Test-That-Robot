#include "world.h"

World::World() {}


void World::Init(QOpenGLExtraFunctions *f)
{
    this->f = f;

    box = std::make_unique<Mesh>(Mesh(Mesh::type::BOX, {1.0f, 1.0f, 1.0f}));
    box->Init();

    robotMesh = std::make_unique<Mesh>(Mesh( {-0.4f, -0.4f, 0.0f,      1.0f, 0.0f, 1.0f,
                           -0.4f, 0.4f, 0.0f,             1.0f, 0.0f, 1.0f,
                           0.4f, 0.4f, 0.0,               1.0f, 0.0f, 1.0f,
                           0.4f, -0.4f, 0.0,              1.0f, 0.0f, 1.0f,
                           0.0f, float(0.3*sqrt(3)/3), 0.0f, 1.0f, 1.0f, 0.0f,
                           0.3f/2.0, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f,
                           -0.3f/2.0f, float(-0.3*sqrt(3)/6), 0.0f, 1.0f, 1.0f, 0.0f},{0,1,2,2,0,3,4,5,6 }));

    robotMesh->Init();

    primitives.reserve(128);

    addRobot();
    addBox();

    grid.Init(20, 20, 1.0f, 2);
    
    // Первоначальное формирование occupancy grid
    updateOccupancyGrid();
}

void World::Update(float dt)
{
    bool needGridUpdate = false;
    
    for (size_t i = 0; i < primitives.size(); ++i) {
        // Сохраняем старую позицию для проверки изменений
        QMatrix4x4 oldMatrix = primitives[i]->modelMatrix;
        
        primitives[i]->update(dt);
        
        // Проверяем, изменилась ли позиция объекта
        if (oldMatrix != primitives[i]->modelMatrix) {
            needGridUpdate = true;
            qDebug() << "Object" << i << "moved, need grid update";
        }
    }

    // Тестируем один раз
    static bool tested = false;
    if (!tested && primitives.size() > 1) {
        testAABB();
        testOccupancyGrid();
        tested = true;
    }

    // Обновляем сетку ТОЛЬКО при необходимости
    if (needGridUpdate) {
        updateOccupancyGrid();
    }
}

void World::testAABB()
{
    qDebug() << "=== AABB Testing ===";

    // Тестируем робота (сложная геометрия)
    if (!primitives.empty()) {
        AABB robotAABB = primitives[0]->getAABB();
        qDebug() << "Robot AABB:";
        qDebug() << "  Min:" << robotAABB.min;
        qDebug() << "  Max:" << robotAABB.max;
        qDebug() << "  Size:" << robotAABB.getSize();
    }

    // Тестируем box (простая геометрия)
    if (primitives.size() > 1) {
        AABB boxAABB = primitives[1]->getAABB();
        qDebug() << "Box AABB:";
        qDebug() << "  Min:" << boxAABB.min;
        qDebug() << "  Max:" << boxAABB.max;
        qDebug() << "  Size:" << boxAABB.getSize();
    }
}

void World::testOccupancyGrid()
{
    qDebug() << "=== Testing Initial Positions ===";
    
    for (size_t i = 0; i < primitives.size(); ++i) {
        QVector3D pos = primitives[i]->modelMatrix.column(3).toVector3D();
        AABB aabb = primitives[i]->getAABB();
        
        qDebug() << "Object" << i << "(" << (primitives[i]->isRobot ? "Robot" : "Box") << "):";
        qDebug() << "  Position:" << pos;
        qDebug() << "  AABB:" << aabb.min << "to" << aabb.max;
    }
}

void World::updateOccupancyGrid()
{
    qDebug() << "=== Updating Occupancy Grid ===";
    qDebug() << "Total objects:" << primitives.size();
    
    grid.Clear();
    
    for (size_t i = 0; i < primitives.size(); ++i) {
        qDebug() << "Processing object" << i << "isRobot:" << primitives[i]->isRobot;
        
        // Пропускаем робота
        if (primitives[i]->isRobot) {
            qDebug() << "Skipping robot at index" << i;
            continue;
        }
        
        // Получаем позицию объекта
        QVector3D worldPosition = primitives[i]->modelMatrix.column(3).toVector3D();
        
        // Получаем локальный AABB
        AABB localAABB = primitives[i]->getAABB();
        
        if (!localAABB.isValid()) {
            qDebug() << "Object" << i << "has invalid AABB";
            continue;
        }

        // Трансформируем AABB в мировые координаты
        QVector3D worldMin = worldPosition + localAABB.min;
        QVector3D worldMax = worldPosition + localAABB.max;
        
        qDebug() << "Object" << i << ":";
        qDebug() << "  World position:" << worldPosition;
        qDebug() << "  Local AABB: min=" << localAABB.min << "max=" << localAABB.max;
        qDebug() << "  World AABB: min=" << worldMin << "max=" << worldMax;

        // ИСПРАВЛЯЕМ: Правильное преобразование координат
        // Сетка: 20x20, от -10 до +10 по каждой оси
        // Grid[0,0] соответствует миру(-10, +10), Grid[19,19] = мир(+10, -10)
        
        int minGridX = static_cast<int>(std::floor(worldMin.x() + 10.0f));
        int minGridY = static_cast<int>(std::floor(worldMin.y() + 10.0f));
        int maxGridX = static_cast<int>(std::floor(worldMax.x() + 10.0f));
        int maxGridY = static_cast<int>(std::floor(worldMax.y() + 10.0f));

        qDebug() << "  Grid coordinates before clamp: [" << minGridX << "," << minGridY 
                 << "] to [" << maxGridX << "," << maxGridY << "]";

        // Ограничиваем границами сетки (0-19)
        minGridX = std::max(0, std::min(minGridX, 19));
        minGridY = std::max(0, std::min(minGridY, 19));
        maxGridX = std::max(0, std::min(maxGridX, 19));
        maxGridY = std::max(0, std::min(maxGridY, 19));

        qDebug() << "  Grid coordinates after clamp: [" << minGridX << "," << minGridY 
                 << "] to [" << maxGridX << "," << maxGridY << "]";

        // Заполняем ячейки
        int cellsSet = 0;
        for (int x = minGridX; x <= maxGridX; ++x) {
            for (int y = minGridY; y <= maxGridY; ++y) {
                grid.SetCell(x, y, 1);
                cellsSet++;
            }
        }
        
        qDebug() << "  Set" << cellsSet << "cells for object" << i;
    }

    grid.Update();
    qDebug() << "=== Grid Update Complete ===";
}

void World::Draw(Shader *dShader, Camera &camera)
{
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    grid.Draw(camera);
    grid.DrawGrid(camera);

    for (size_t i = 0; i < primitives.size(); ++i) {
        dShader->Activate();

        bool isSelected = (static_cast<int>(i) == selectedObjectIndex);
        GLint loc = f->glGetUniformLocation(dShader->ID, "isSelected");
        f->glUniform1i(loc, isSelected ? 1 : 0);
        primitives[i]->update(0.016f);
        primitives[i]->Draw(dShader);
    }
}



void World::initPicking(int w, int h)
{
    picking.init(w,h);
}

void World::paintPicking(Shader *fShader)
{
    picking.enableWrite();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint id =0;

    for(auto& obj :primitives){
        fShader->Activate();

        GLint loc = f->glGetUniformLocation(fShader->ID, "objectID");
        GLint mvp = f->glGetUniformLocation(fShader->ID, "model");
        f->glUniform1ui(loc, id+1);
        f->glUniformMatrix4fv(mvp,1, GL_FALSE, obj->modelMatrix.constData());


        obj->Draw(fShader);
        id++;
    }
    picking.disableWrite();
}

void World::addRobot()
{
    primitives.emplace_back(std::make_unique<Robot>(robotMesh.get()));
    primitives.back()->initialize();
}

void World::addBox()
{
    primitives.emplace_back(std::make_unique<Object>(box.get()));
    primitives.back()->initialize();
    
    // Убираем автоматическое позиционирование - пусть пользователь сам размещает
    QVector3D pos = primitives.back()->modelMatrix.column(3).toVector3D();
    qDebug() << "Added box at position:" << pos;
    
    // Обновляем сетку при добавлении объекта
    updateOccupancyGrid();
}

void World::deleteObject()
{
    if(selectedObjectIndex == -1) return;

    if(primitives.size() == 1){
        primitives.clear();
    } else {
        primitives.erase(primitives.begin() + selectedObjectIndex);
    }
    selectedObjectIndex = -1;
    
    // Обновляем сетку при удалении объекта
    updateOccupancyGrid();
}

void World::selectObject(int index)
{
    if (index >= 0 && index < static_cast<int>(primitives.size())) {
        selectedObjectIndex = index;
    } else {
        selectedObjectIndex = -1;
    }
    qDebug() << "selected " << selectedObjectIndex;
}

void World::translateObject(float x, float y, Camera & camera){
    if(selectedObjectIndex == -1) return;

    float speed = camera.getZpos() * 0.0011f;
    
    QVector3D oldPos = primitives[selectedObjectIndex]->modelMatrix.column(3).toVector3D();
    primitives[selectedObjectIndex]->Translate(x*speed, y*speed, 0.0f);
    QVector3D newPos = primitives[selectedObjectIndex]->modelMatrix.column(3).toVector3D();
    
    qDebug() << "Object" << selectedObjectIndex << "moved from" << oldPos << "to" << newPos;
    
    // Обновляем сетку при перемещении объекта
    updateOccupancyGrid();
}

void World::startRobot(int key)
{
    primitives.front()->start(key);
    qDebug() << "robot START \n";
}

void World::stopRobot()
{
    primitives.front()->stop();
    qDebug() << "robot STOP \n";
}
