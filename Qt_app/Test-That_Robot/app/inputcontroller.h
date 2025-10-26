#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QPoint>
#include "core/world.h"
#include "renderer/scene_renderer.h"

// predeclare classes to reduce dependencies
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class SceneRenderer;

class InputController {
public:
    InputController();
    ~InputController();
    void init(World* world, SceneRenderer* renderer);

    void handleMousePress(QMouseEvent* e, float dpr);
    void handleMouseRelease(QMouseEvent* e);
    void handleMouseMove(QMouseEvent* e);
    void handleWheelEvent(QWheelEvent* e, float dpr);
    void handleKeyPress(QKeyEvent* e);
    void handleKeyRelease(QKeyEvent* e);

private:
    World* _world;       // pointer to the world
    SceneRenderer* _renderer; // pointer to the renderer
    QMouseEvent *_e;

    QPoint _lastMousePos;
    QVector3D _lastMouseWorldPos;
    bool _leftMousePressed = false;
    bool _rightMousePressed = false;

    //fun
    QVector3D getMouseWorldPos(const QPoint pos, float dpr);
};

#endif // INPUTCONTROLLER_H
