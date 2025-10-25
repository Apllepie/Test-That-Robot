#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QPoint>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVector3D>
#include <QMatrix4x4>

#include "camera.h"
#include "scene.h"

class InputController {
public:
    InputController() = default;
    explicit InputController(Scene* scn) : camera(&scn->camera), scene(scn) {}
    ~InputController() = default;

    void wheel(QWheelEvent* e);
    void mousePress(QMouseEvent* e, float dpr, int widgetHeight);
    void mouseRelease(QMouseEvent* e);
    void mouseMove(QMouseEvent* e);
    void keyPress(QKeyEvent* e);
    void keyRelease(QKeyEvent *e);
    bool isSpacePressed() {return spacePressed;}

private:
    Camera* camera = nullptr;
    Scene*  scene  = nullptr;

    bool leftPressed  = false;
    bool rightPressed = false;
    bool spacePressed = false;
    QPoint lastPos;
    QVector3D mouseWorldPos;

    QVector3D getMouseWorldPos(QMouseEvent* e);
    void pickAt(float dpr, int widgetHeight);
};

#endif // INPUTCONTROLLER_H
