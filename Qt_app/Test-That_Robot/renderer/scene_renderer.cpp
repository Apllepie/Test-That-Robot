#include "scene_renderer.h"
#include "qopenglfunctions.h"
#include <algorithm>


SceneRenderer::SceneRenderer() {}

SceneRenderer::~SceneRenderer()
{

}

void SceneRenderer::init()
{
    initializeOpenGLFunctions();
    _camera.Init();

    _gridRenderer = std::make_unique<GridRenderer>();
    _gizmo = std::make_unique<GizmoHandler>();

    _defaultShader =  std::make_unique<Shader>(":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");
    _frameShader = std::make_unique<Shader>(":/Shaders/shaders/pick.vert",":/Shaders/shaders/pick.frag");
    _gridShader = std::make_unique<Shader>(":/Shaders/shaders/occupancy.vert",":/Shaders/shaders/occupancy.frag");
    _occupancyShader = std::make_unique<Shader>(":/Shaders/shaders/occupancy.vert",":/Shaders/shaders/occupancy.frag");
    _outlineShader = std::make_unique<Shader>(":/Shaders/shaders/outline.vert",":/Shaders/shaders/outline.frag");


}

void SceneRenderer::finishInitialization(const World &world)
{
    _gridRenderer->init(*world.getOccupancyGrid());
}

void SceneRenderer::resize(int w, int h)
{
    _w = int(w );
    _h = int(h );
    _picking.init(w, h);
    glViewport(0, 0, w, h);

    qDebug() << "Resized to" << w << "x" << h;
    _camera.changeProjection(w, h, 45.0f, 0.1f, 100.0f);
}

void SceneRenderer::render(const World& world)
{


    glClearColor(0.0f, 0.3f,0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    //glEnable(GL_DEPTH_TEST);
    renderForPicking(world);

    _camera.Activate(_defaultShader.get());


    const OccupancyGrid* grid = world.getOccupancyGrid();
    if (grid) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        _gridRenderer->render(*grid, _camera);

        glDisable(GL_BLEND);
    }

    const auto& primitives = world.getPrimitives();
    for (size_t i = 0; i < primitives.size(); ++i) {
        _defaultShader->Activate();

        // check if the current object is selected
        bool isSelected = (static_cast<int>(i) == world.getSelectedObjectIndex());
        if (isSelected) {
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF); // Включаем запись в stencil
            glClear(GL_STENCIL_BUFFER_BIT); // Очищаем stencil буфер перед отрисовкой нового объекта
        }

        // --- 1. Рисуем основной объект ---
        // Он также заполнит stencil buffer, если isSelected == true
        _defaultShader->Activate();
        _camera.Activate(_defaultShader.get());
        GLint modelLoc = glGetUniformLocation(_defaultShader->ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, primitives[i]->getModelMatrix().constData());
        primitives[i]->getMesh()->Draw();

        if (isSelected) {
            // --- 2. Рисуем обводку ---
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Рисуем только там, где в буфере НЕ 1
            glStencilMask(0x00); // Отключаем запись в stencil
            glDisable(GL_DEPTH_TEST); // Обводка всегда поверх

            _outlineShader->Activate();
            _camera.Activate(_outlineShader.get());

            GLint outlineModelLoc = glGetUniformLocation(_outlineShader->ID, "model");
            glUniformMatrix4fv(outlineModelLoc, 1, GL_FALSE, primitives[i]->getModelMatrix().constData());

            GLint thicknessLoc = glGetUniformLocation(_outlineShader->ID, "outline_thickness");
            glUniform1f(thicknessLoc, 0.03f);

            primitives[i]->getMesh()->Draw();

            // Возвращаем все в исходное состояние
            glDisable(GL_STENCIL_TEST);

            // Рисуем гизмо только для выбранного объекта
            if(dynamic_cast<Obstacle*>(primitives[i].get())) {
                _defaultShader->Activate(); // Гизмо рисуется обычным шейдером
                _camera.Activate(_defaultShader.get());
                _gizmo->drawGizmo(&_camera, _defaultShader.get(), primitives[i]->getModelMatrix());
            }
        }


    }
}



QVector2D SceneRenderer::getWindowSize()
{
    return QVector2D(_w, _h);
}

void SceneRenderer::renderForPicking(const World &world)
{
    _picking.enableWrite();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto &primitives = world.getPrimitives();
    for (size_t i =0 ; i < primitives.size(); i ++ ){
        _frameShader->Activate();
        _camera.Activate(_frameShader.get());
        uint objectID = static_cast<uint>(i + 1);

        GLint loc = glGetUniformLocation(_frameShader->ID, "objectID");
        GLuint handleIdLoc = glGetUniformLocation(_frameShader->ID, "handleID");
        GLint mvp = glGetUniformLocation(_frameShader->ID, "model");
        glUniform1ui(loc, objectID);
        glUniform1ui(handleIdLoc, 0);
        glUniformMatrix4fv(mvp,1, GL_FALSE, primitives[i]->getModelMatrix().constData());
         primitives[i]->getMesh()->Draw();
        // if (isSelected && !dynamic_cast<Robot*>(primitives[i].get())) {
        //      const QMatrix4x4& modelMatrix = primitives[i]->getModelMatrix();
        //     _gizmo->drawGizmoForPicking( _frameShader.get(), modelMatrix, objectID);
        // }
         if (static_cast<int>(i) == world.getSelectedObjectIndex() && dynamic_cast<Obstacle*>(primitives[i].get())) {
             const QMatrix4x4& modelMatrix = primitives[i]->getModelMatrix();
             _gizmo->drawGizmoForPicking(_frameShader.get(), modelMatrix, objectID);
             qDebug() << "DRAWWWW HANDLESR";
         }
    }

    _picking.disableWrite();
}

PickingObject::pixelInfo SceneRenderer::pickAt(int x, int y)
{
    PickingObject::pixelInfo pixel =  takePixel(x, y);
    return pixel ;

}

PickingObject::pixelInfo SceneRenderer::takePixel(uint x, uint y)
{
    uint px = x *_dpr;
    uint py = (_h - y * _dpr);
    return  _picking.readPixel(px,  py );
}




