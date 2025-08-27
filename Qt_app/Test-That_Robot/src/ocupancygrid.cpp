#include "ocupancygrid.h"
#include <QOpenGLContext>


OcupancyGrid::OcupancyGrid()
{

}

OcupancyGrid::~OcupancyGrid()
{
    Cleanup();
}



void OcupancyGrid::Init(int w, int h, float cSize, int res)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context for OcupancyGrid");
    }
    f = context->extraFunctions();

    width = w;
    height = h;
    cellSize = cSize;
    resolution = res;

    // Total number of sub-cells along each axis
    int totalWidth = width * resolution;
    int totalHeight = height * resolution;
    grid.assign(totalWidth * totalHeight, 0);

    shader = new Shader(":/Shaders/shaders/occupancy.vert", ":/Shaders/shaders/occupancy.frag");

    // Размер одной суб-ячейки
    float subCellSize = cellSize / resolution;
    float s = subCellSize / 2.0f; // Half the size of the sub-cell
    GLfloat quadVertices[] = {
        -s, -s,
         s, -s,
        -s,  s,
         s,  s,
    };

    vao.Init();
    quadVBO.Init(quadVertices, sizeof(quadVertices));

    vao.Bind();
    vao.linkAttribut(quadVBO, 0, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);

    // memory for all cells
    instanceVBO.Init(nullptr, totalWidth * totalHeight * 5 * sizeof(float), GL_DYNAMIC_DRAW);
    vao.linkAttribut(instanceVBO, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)0); // aOffset
    vao.linkAttribut(instanceVBO, 2, 3, GL_FLOAT, 5 * sizeof(float), (void*)(2 * sizeof(float))); // aColor

    f->glVertexAttribDivisor(1, 1); // aOffset
    f->glVertexAttribDivisor(2, 1); // aColor

    vao.Unbind();

    initGrid();
    dirty = true; 
}

void OcupancyGrid::Update()
{
    if (width == 0 || height == 0) return;

    int totalWidth = width * resolution;
    int totalHeight = height * resolution;
    float subCellSize = cellSize / resolution;

    instanceData.clear();
    //draw only occupied cells

    // Starting point of the entire large grid
    float startX = -(width * cellSize) / 2.0f;
    float startY = -(height * cellSize) / 2.0f;

    for (int y = 0; y < totalHeight; ++y) {
        for (int x = 0; x < totalWidth; ++x) {
            // Check if the cell is occupied
            if (grid[y * totalWidth + x] == 1) {
                // Offset: compute the center of each sub-cell
                instanceData.push_back(startX + x * subCellSize + subCellSize / 2.0f);
                instanceData.push_back(startY + y * subCellSize + subCellSize / 2.0f);

                // Color
                instanceData.push_back(1.0f); // R
                instanceData.push_back(0.0f); // G
                instanceData.push_back(0.0f); // B
            }
        }
    }
    
    if (!instanceData.empty()) {
        instanceVBO.Update(instanceData.data(), instanceData.size() * sizeof(float));
    }
}

void OcupancyGrid::Draw(Camera& camera)
{
    // If the grid data has changed, update the instance data
    if (dirty) {
        Update();
        dirty = false; // Reset the flag
    }

    if (!shader || instanceData.empty()) return;

    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->Activate();
    camera.Activate(shader);

    vao.Bind();
    // Draw as many instances as we have occupied cells
    GLsizei instanceCount = instanceData.size() / 5; // 5 floats per instance
    f->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instanceCount);
    vao.Unbind();

    f->glDisable(GL_BLEND);
}

void OcupancyGrid::DrawGrid(Camera &camera)
{

    //drawing grid and axis
    gridShader->Activate();
    camera.Activate(gridShader);
    QMatrix4x4 gridModel; gridModel.setToIdentity();
    GLint uModel = f->glGetUniformLocation(gridShader->ID, "model");
    f->glUniformMatrix4fv(uModel, 1, GL_FALSE, gridModel.constData());
    gridVAO.Bind();
    f->glLineWidth(0.5f);
    f->glDrawArrays(GL_LINES, 0,  gridVertexCount);
    gridVAO.Unbind();
    axisVAO.Bind();
    f->glLineWidth(1.0f);
    f->glDrawArrays(GL_LINES, 0, axisVertexCount);
    axisVAO.Unbind();

}

void OcupancyGrid::SetCell(int x, int y, int value)
{
    int totalWidth = width * resolution;
    int totalHeight = height * resolution;
    if (x >= 0 && x < totalWidth && y >= 0 && y < totalHeight) {
        if (grid[y * totalWidth + x] != value) { // Update only if the value changes
            grid[y * totalWidth + x] = value;
            dirty = true; // Set the flag that the data has changed
        }
    }
}

void OcupancyGrid::initGrid()
{
 gridShader = new Shader(":/Shaders/shaders/grid.vert",":/Shaders/shaders/grid.frag");

    // grid
    float startX = -((width) * cellSize) / 2.0f;
    float startY = -((height) * cellSize) / 2.0f;

    const float size = width / 2;
    std::vector<GLfloat> grid;  // pos(3) + color(3) = 6 floats for vertex

    auto addLine = [&](float x1,float y1,float x2,float y2,float r,float g,float b) {
        grid.insert(grid.end(), {x1,y1,0.0f, r,g,b,  x2,y2,0.0f, r,g,b});
    };

    // thin lines
    for (int y = 0; y <=height; y ++)
        addLine(startX, startY + y *cellSize , -startX,  startY + y *cellSize, 0.4f,0.4f,0.4f);
    for (int x = 0; x <= width; x ++)
        addLine(startX + x*cellSize, startY,  startX + x*cellSize, -startY, 0.4f,0.4f,0.4f);

    gridVertexCount = static_cast<GLsizei>(grid.size() / 6);
    gridVAO.Init();
    gridVBO.Init(grid.data(), grid.size() * sizeof(GLfloat));
    gridVAO.Bind();
    gridVAO.linkAttribut(gridVBO, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
    gridVAO.linkAttribut(gridVBO, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));

    gridVAO.Unbind();
    gridVBO.Unbind();

    // 2) axes red y green x
    std::vector<GLfloat> axes;
    auto addAxis = [&](float x1,float y1,float x2,float y2,float r,float g,float b){
        axes.insert(axes.end(), {x1,y1,0.0f, r,g,b,  x2,y2,0.0f, r,g,b});
    };
    addAxis(-size, 0, size, 0, 1.0f,0.1f,0.1f); // X
    addAxis(0, -size, 0, size, 0.1f,1.0f,0.1f); // Y
    axisVertexCount = static_cast<GLsizei>(axes.size() / 6);
    axisVAO.Init();
    axisVBO.Init(axes.data(), axes.size() * sizeof(GLfloat));
    axisVAO.Bind();
    axisVAO.linkAttribut(axisVBO, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
    axisVAO.linkAttribut(axisVBO, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));

    axisVAO.Unbind();
    axisVBO.Unbind();
}

void OcupancyGrid::Cleanup()
{
    if(shader){ shader->Delete(); delete shader; }
    if(gridShader) { gridShader->Delete(); delete gridShader ;}
    axisVAO.Delete();
    axisVBO.Delete();
    gridVAO.Delete();
    gridVBO.Delete();
    vao.Delete();
    quadVBO.Delete();
    instanceVBO.Delete();
}
