#include "grid_renderer.h"

GridRenderer::GridRenderer() {}

GridRenderer::~GridRenderer()
{
    glDeleteVertexArrays(1, &_gridCellVAO);
    glDeleteBuffers(1, &_gridCellVBO);
    glDeleteBuffers(1, &_gridInstanceVBO);
    glDeleteVertexArrays(1, &_lineGridVAO);
    glDeleteBuffers(1, &_lineGridVBO);
}

void GridRenderer::init(const OccupancyGrid& grid)
{
    initializeOpenGLFunctions();

    _occupancyShader = std::make_unique<Shader>(":/Shaders/shaders/occupancy.vert", ":/Shaders/shaders/occupancy.frag");
    float cellSize = grid.getCellSize();
    //float cellSize = 1.0f;
    float s = cellSize / 2.0f;
    float vertices[] = { -s, -s, s, -s, s, s, s, s, -s, s, -s, -s };


    glGenVertexArrays(1, &_gridCellVAO);
    glGenBuffers(1, &_gridCellVBO);
    glBindVertexArray(_gridCellVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _gridCellVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glGenBuffers(1, &_gridInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _gridInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);

    // grid lines
    _lineShader = std::make_unique<Shader>(":/Shaders/shaders/grid.vert", ":/Shaders/shaders/grid.frag");

    std::vector<float> lineVertices;
    int gridWidth = grid.getWidth();
    int gridHeight = grid.getHeight();

    QVector2D minCorner = grid.gridToWorld(0, 0) - QVector2D(cellSize/2.0f, cellSize/2.0f);
    QVector2D maxCorner = grid.gridToWorld(gridWidth-1, gridHeight-1) + QVector2D(cellSize/2.0f, cellSize/2.0f);

    // Горизонтальные линии
    for (int i = 0; i <= gridHeight * cellSize; ++i) {
        float y = minCorner.y() + i * 1.0f; // world cel size
        lineVertices.push_back(minCorner.x()); lineVertices.push_back(y);
        lineVertices.push_back(maxCorner.x()); lineVertices.push_back(y);
    }
    // Вертикальные линии
    for (int i = 0; i <= gridWidth * cellSize; ++i) {
        float x = minCorner.x() + i * 1.0f; // world cel size
        lineVertices.push_back(x); lineVertices.push_back(minCorner.y());
        lineVertices.push_back(x); lineVertices.push_back(maxCorner.y());
    }
    _lineVertexCount = lineVertices.size() / 2;

    glGenVertexArrays(1, &_lineGridVAO);
    glGenBuffers(1, &_lineGridVBO);
    glBindVertexArray(_lineGridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _lineGridVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void GridRenderer::render(const OccupancyGrid& grid, const Camera& camera)
{
    _lineShader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(_lineShader->ID, "view"), 1, GL_FALSE, camera.view.constData());
    glUniformMatrix4fv(glGetUniformLocation(_lineShader->ID, "projection"), 1, GL_FALSE, camera.projection.constData());
    glUniform3f(glGetUniformLocation(_lineShader->ID, "uColor"), 0.2f, 0.4f, 0.4f); // Темный серо-зеленый цвет

    glBindVertexArray(_lineGridVAO);
    glDrawArrays(GL_LINES, 0, _lineVertexCount);

    // --- Фаза 2: Рисуем ЗАНЯТЫЕ ЯЧЕЙКИ (как и раньше) ---
    std::vector<std::pair<QVector2D, unsigned char>> occupiedPositionsP;
    occupiedPositionsP =  grid.getOccupiedCellWorldPositions();

    std::vector<QVector2D> occupiedPositions;
     std::vector<QVector2D> NoccupiedPositions;
    for(size_t i =0; i < occupiedPositionsP.size(); ++i){
        if(occupiedPositionsP[i].second >160 ){
        occupiedPositions.push_back(occupiedPositionsP[i].first);
        }else {
            NoccupiedPositions.push_back(occupiedPositionsP[i].first);
        }
    }

    if (occupiedPositions.empty()) {
        glBindVertexArray(0); // Отвязываем VAO линий и выходим
        return;
    }

    _occupancyShader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(_occupancyShader->ID, "view"), 1, GL_FALSE, camera.view.constData());
    glUniformMatrix4fv(glGetUniformLocation(_occupancyShader->ID, "projection"), 1, GL_FALSE, camera.projection.constData());
    glUniform3f(glGetUniformLocation(_occupancyShader->ID, "uColor"), 1.0f, 0.2f, 0.2f);

    glBindBuffer(GL_ARRAY_BUFFER, _gridInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, occupiedPositions.size() * sizeof(QVector2D), occupiedPositions.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(_gridCellVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, occupiedPositions.size());

    glUniform3f(glGetUniformLocation(_occupancyShader->ID, "uColor"), 1.0f, 0.75f, 0.2f);
    glBindBuffer(GL_ARRAY_BUFFER, _gridInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, NoccupiedPositions.size() * sizeof(QVector2D), NoccupiedPositions.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(_gridCellVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, NoccupiedPositions.size());

    glBindVertexArray(0);
}
