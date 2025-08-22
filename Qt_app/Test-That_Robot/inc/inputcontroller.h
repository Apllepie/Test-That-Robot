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
    explicit InputController(Camera* cam, Scene* scn) : camera(cam), scene(scn) {}
    ~InputController() = default;

    void wheel(QWheelEvent* e);
    void mousePress(QMouseEvent* e, float dpr, int widgetHeight);
    void mouseRelease(QMouseEvent* e);
    void mouseMove(QMouseEvent* e);
    void keyPress(QKeyEvent* e);

private:
    Camera* camera = nullptr;
    Scene*  scene  = nullptr;

    bool leftPressed  = false;
    bool rightPressed = false;
    QPoint lastPos;
    QVector3D mouseWorldPos;

    QVector3D getMouseWorldPos(QMouseEvent* e);
    void pickAt(float dpr, int widgetHeight);
};

#endif // INPUTCONTROLLER_H
