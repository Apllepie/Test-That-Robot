#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include <QOpenGLExtraFunctions>
#include <memory>
#include "core/world.h"
#include "grid_renderer.h"
#include "renderer/camera.h"
#include "renderer/shaderclass.h"
#include "renderer/pickingobject.h"
#include "gizmohandler.h"



class SceneRenderer : protected QOpenGLExtraFunctions
{
private:
    int _w, _h;
    float _dpr;
    Camera _camera;
    PickingObject _picking;

    std::unique_ptr<Shader> _defaultShader;
    std::unique_ptr<Shader> _frameShader;
    std::unique_ptr<Shader> _gridShader;
    std::unique_ptr<Shader> _occupancyShader;
    std::unique_ptr<Shader> _outlineShader;

    std::unique_ptr<GridRenderer> _gridRenderer;
    std::unique_ptr<GizmoHandler> _gizmo;

    //functions
     PickingObject::pixelInfo takePixel(uint x, uint y);

public:
    SceneRenderer();
    ~SceneRenderer();


    void init();
    void finishInitialization(const World& world);
    void resize(int w, int h);
    void render(const World& world);

    Camera* getCamera() { return &_camera; }
    QVector2D getWindowSize();
    QPoint getWinSize(){return QPoint(_w, _h);}

    void renderForPicking(const World& world);

    PickingObject::pixelInfo pickAt(int x, int y);
    float getDevicePixelRatio() const { return _dpr; }
    void setDevicePixelRatio(float dpr) {  _dpr = dpr; }
    GizmoHandler* getGizmoHandler() { return _gizmo.get(); }
  




};

#endif // SCENE_RENDERER_H
