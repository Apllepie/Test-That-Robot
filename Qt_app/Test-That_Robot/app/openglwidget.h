#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QTimer>
#include <QElapsedTimer>


#include "core/world.h"
#include "renderer/scene_renderer.h"
#include "app/inputcontroller.h"


class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

public slots:
    void addingBox(); // Slot for the "Add Box" button

protected:
    // --- Main QOpenGLWidget methods ---
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // --- Input event handlers ---
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // --- Core system components ---
    World _world;
    SceneRenderer _renderer;
    InputController _inputController;

    // --- Timers for animation and physics ---
    QTimer* _frameTimer;      // Forces the widget to repaint at ~60 FPS
    QElapsedTimer _simTimer;  // Measures time between frames (dt)
};

#endif // OPENGLWIDGET_H
