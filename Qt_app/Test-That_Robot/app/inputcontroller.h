#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QPoint>

// predeclare classes to reduce dependencies
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
namespace core { class World; }
class SceneRenderer;

class InputController {
public:
    InputController(core::World* world, SceneRenderer* renderer);

    void handleMousePress(QMouseEvent* e, float dpr, int widgetHeight);
    void handleMouseRelease(QMouseEvent* e);
    void handleMouseMove(QMouseEvent* e);
    void handleWheelEvent(QWheelEvent* e);
    void handleKeyPress(QKeyEvent* e);
    void handleKeyRelease(QKeyEvent* e);

private:
    core::World* _world;       // pointer to the world
    SceneRenderer* _renderer; // pointer to the renderer

    QPoint _lastMousePos;
    bool _leftMousePressed = false;
    bool _rightMousePressed = false;
};

#endif // INPUTCONTROLLER_H