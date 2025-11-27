#ifndef GRID_RENDERER_H
#define GRID_RENDERER_H

#include <QOpenGLExtraFunctions>
#include <memory>
#include "core/occupancy_grid.h"
#include "renderer/camera.h"
#include "renderer/shaderclass.h"


class GridRenderer : protected QOpenGLExtraFunctions
{

private:
    std::unique_ptr<Shader> _occupancyShader;
    GLuint _gridCellVAO = 0;
    GLuint _gridCellVBO = 0;
    GLuint _gridInstanceVBO = 0;

    std::unique_ptr<Shader> _lineShader;
    GLuint _lineGridVAO = 0;
    GLuint _lineGridVBO = 0;
    int _lineVertexCount = 0;

    GLuint _axesVAO = 0;
    GLuint _axesVBO = 0;

public:
    GridRenderer();
    ~GridRenderer();

    void init(const OccupancyGrid& grid);
    void render(const OccupancyGrid& grid, const Camera& camera);
};

#endif // GRID_RENDERER_H

