#include "openglwidget.h"
#include <QMouseEvent>
#include <QKeyEvent>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)//,
      //m_inputController(&_world, &_renderer) // connect InputController to World and Renderer
{
    setFocusPolicy(Qt::StrongFocus); // Enable keyboard focus for the widget

    _frameTimer = new QTimer(this);
    connect(_frameTimer, &QTimer::timeout, this, QOverload<>::of(&OpenGLWidget::update));
    _frameTimer->start(16); // Target ~60 FPS
}

OpenGLWidget::~OpenGLWidget() {}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions(); // Initialize the widget itself

    _world.setLogCallback([this](const std::string& msg, World::LogType type) {
        // Вызываем сигнал. Т.к. вызов может быть из другого потока (теоретически),
        // сигнал безопасен. Cast enum to int for simplicity in signals.
        emit logMessage(QString::fromStdString(msg), static_cast<int>(type));
    });
    _world.setStatusCallback([this](const std::string& msg) {
        // Отправляем сигнал в главное окно
        emit statusMessage(QString::fromStdString(msg));
    });

    // Pass control of initialization to child components
    _world.init();
    _renderer.init(); // Pass this for access to OpenGL functions
    _renderer.finishInitialization(_world);
    _world.setGizmoHandler(_renderer.getGizmoHandler());
    _inputController.init(&_world, &_renderer, devicePixelRatioF());

    _simTimer.start(); // Start the simulation timer

}

void OpenGLWidget::resizeGL(int w, int h)
{
    _renderer.resize(w * devicePixelRatio(), h * devicePixelRatio());
    _renderer.setDevicePixelRatio(devicePixelRatio());
}

void OpenGLWidget::paintGL()
{
    // Calculate the time elapsed since the last frame
    float dt = _simTimer.restart() / 1000.0f;

    // 1. Update the world logic (physics, movement)
    _world.update(dt);

    // 2. Render the current state of the world
    _renderer.render(_world);
}

void OpenGLWidget::addingBox()
{
    // Send command to the world
    _world.addBoxAt(0, 0, 1, 1);
}

void OpenGLWidget::addingP()
{
    _world.addBoxAt(0, 0, 1, 4);
}

void OpenGLWidget::addingC()
{
    _world.addCircleAt(0,0,1);
}

void OpenGLWidget::addingT()
{
    _world.addTriangleAt(0,0, 1);
}

void OpenGLWidget::addingRobot()
{
    _world.addRobotAt(0,0);
}

void OpenGLWidget::runScript(const QString &scriptCode)
{
    _world.runMainScript(scriptCode.toStdString());
}

QJsonObject OpenGLWidget::getWorldState() const
{
    return _world.saveState();
}

void OpenGLWidget::loadWorldState(const QJsonObject &state)
{
    _world.loadState(state);
}



// --- Delegating all input events to InputController ---

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    _inputController.handleWheelEvent(event);
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    _inputController.handleMousePress(event);
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    _inputController.handleMouseRelease(event);
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    _inputController.handleMouseMove(event);
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    _inputController.handleKeyPress(event);
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    _inputController.handleKeyRelease(event);
}
