#include "inputcontroller.h"
#include "qevent.h"
#include <QDebug>





InputController::InputController()
{

}

InputController::~InputController()
{
    _world = nullptr;
    _renderer = nullptr;
}

void InputController::init(World *world, SceneRenderer *renderer, float dpr)
{
    _world  = world;
    _renderer = renderer;
    _dpr = dpr;
}

void InputController::handleMousePress(QMouseEvent *e) {
    _lastMousePos = e->pos();
    _lastMouseWorldPos = getMouseWorldPos(e->pos(), _dpr);

   if (e->button() == Qt::LeftButton) {
        _leftMousePressed = true;
       _renderer->pickAt(*_world, _lastMousePos.x(), _lastMousePos.y());
  }
    else if (e->button() == Qt::RightButton) {
        _rightMousePressed = true;
      _world->setRobotDestination(_lastMouseWorldPos);
    }
  else if(e->button() == Qt::MiddleButton){
        _middleMosePressed = true;
    }
    
}

void InputController::handleMouseRelease(QMouseEvent *e) {
    if(e->button() == Qt::RightButton){
        _rightMousePressed = false;
    }
    else if(e->button() == Qt::LeftButton){
        _leftMousePressed = false;
    }
    else if(e->button() == Qt::MiddleButton){
        _middleMosePressed = false;
    }
}

void InputController::handleMouseMove(QMouseEvent *e) {
    if(_middleMosePressed){
        QPoint delta = e->pos() - _lastMousePos;
        _lastMousePos = e->pos();
        _renderer->getCamera()->Move(delta);
    }
    if (_leftMousePressed && _world->getSelectedObjectIndex() != -1) {
        QVector3D currentMouseWorldPos = getMouseWorldPos(e->pos(), _dpr);
        QVector3D delta = currentMouseWorldPos - _lastMouseWorldPos;
       _lastMouseWorldPos = currentMouseWorldPos;
       _world->translateObject(delta.x(), delta.y());

    }
}

void InputController::handleWheelEvent(QWheelEvent *e)
{

    QVector3D worldPosBeforeZoom = getMouseWorldPos(e->position().toPoint(), _dpr);

    float delta = e->angleDelta().y() / 120.0f;
    _renderer->getCamera()->moveCloser_Away(-delta);

    QVector3D worldPosAfterZoom = getMouseWorldPos(e->position().toPoint(), _dpr);

    QVector3D difference = worldPosBeforeZoom - worldPosAfterZoom;
    _renderer->getCamera()->pan(difference.x(), difference.y());
}

void InputController::handleKeyPress(QKeyEvent *e) {
    if(e->key() == Qt::Key_Backspace or e->key() == Qt::Key_Delete){
        qDebug() << "pressed backspace or delete\n";
        _world->deleteObject();
    }
}

void InputController::handleKeyRelease(QKeyEvent *e)
{

}

QVector3D InputController::getMouseWorldPos(const QPoint pos, float dpr)
{
    float physicalX = pos.x() * dpr;
    float physicalY = pos.y() * dpr;
    float physicalWidth = _renderer->getWinSize().x();
    float physicalHeight = _renderer->getWinSize().y();

    if (physicalHeight == 0) return QVector3D();

    float ndcX = (2.0f * physicalX) / physicalWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * physicalY) / physicalHeight;
    float ndcZ = -1.0f;

    QVector3D ndc(ndcX, ndcY, ndcZ);

    QVector4D clip(ndc, 1.0f);

    bool invertible;
    QMatrix4x4 invertedProjection = _renderer->getCamera()->projection.inverted(&invertible);
    if (!invertible) {
        qWarning() << "Error: projection matrix not invertible!";
        return QVector3D();
    }

    QVector4D eyeCoords = invertedProjection * clip;
    eyeCoords.setZ(-1.0f);
    eyeCoords.setW(0.0f);

    QMatrix4x4 invertedView = _renderer->getCamera()->view.inverted(&invertible);
    if (!invertible) {
        qWarning() << "Error: view matrix can not be inverted!!";
        return QVector3D();
    }
    QVector4D worldRay = invertedView * eyeCoords;
    QVector3D rayDirection = worldRay.toVector3D().normalized();


    QVector3D cameraPos = invertedView.column(3).toVector3D();

    if (rayDirection.z() != 0.0f)
    {
        float t = -cameraPos.z() / rayDirection.z();
        return cameraPos + t * rayDirection;
    }

    // Если луч параллелен (маловероятно), возвращаем пустой вектор
    return QVector3D();

}






