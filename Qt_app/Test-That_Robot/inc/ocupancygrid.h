#ifndef OCUPANCYGRID_H
#define OCUPANCYGRID_H


#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>
#include <vector>

#include "camera.h"
#include "vbo.h"
#include "vao.h"


class OcupancyGrid
{
public:
    OcupancyGrid();
    ~OcupancyGrid();

    int height, width;
    float cellSize = 1.0f;
    int resolution;

    std::vector<int> grid;

    //fun
    void Init(int w, int h, float size, int res);
    void Update();
    void Draw(Camera& camera);
    void DrawGrid(Camera& camera);
    void SetCell(int x, int y, int value);

private:
    bool dirty = true; // check for changes
    QOpenGLExtraFunctions* f = nullptr;
    //
    VAO vao;
    VBO quadVBO;
    VBO instanceVBO;

    Shader* gridShader = nullptr;
    VAO gridVAO; 
    VBO gridVBO;
    GLsizei gridVertexCount = 0;

    VAO axisVAO;
    VBO axisVBO;
    GLsizei axisVertexCount = 0;


    Shader* shader = nullptr;
    std::vector<float> instanceData;

    //fun
    void initGrid();
    void Cleanup();

};

#endif // OCUPANCYGRID_H
