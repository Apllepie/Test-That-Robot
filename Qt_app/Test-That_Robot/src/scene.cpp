#include "scene.h"

Scene::Scene() {}

Scene::~Scene()
{
    for (auto it : primitives){
        it.Destruct();
    }

    if (gridShader) { delete gridShader; gridShader = nullptr; }
    if (gridVBO) {gridVBO->Delete(); gridVBO = nullptr;}
    if (gridVAO) {gridVAO->Delete(); gridVAO = nullptr;}
    if (axisVBO) {axisVBO->Delete(); axisVBO = nullptr;}
    if (axisVAO) {axisVAO->Delete(); axisVAO = nullptr;}
}

void Scene::initialize()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    primitives.emplace_back(Object(Object::type::BOX, {1.0f, 1.0f, 1.0f}));
    qDebug() <<"objects = " <<primitives.size()<<"\n";
    primitives[0].initialize(":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");
    primitives[0].printVertices();

    initGrid();

}

void Scene::resize(int w, int h)
{
    f->glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
    //camera.changeProjection(w, h, 45.0f, 0.1f, 100.0f);
}

void Scene::paint(Camera& camera)
{
    f->glClear(GL_COLOR_BUFFER_BIT);
    //drawing grid and axis
    gridShader->Activate();
    camera.Activate(gridShader);
    QMatrix4x4 gridModel; gridModel.setToIdentity();
    GLint uModel = f->glGetUniformLocation(gridShader->ID, "model");
    f->glUniformMatrix4fv(uModel, 1, GL_FALSE, gridModel.constData());
    gridVAO->Bind();
    f->glLineWidth(0.5f);
    f->glDrawArrays(GL_LINES, 0, gridVertexCount);
    gridVAO->Unbind();
    axisVAO->Bind();
    f->glLineWidth(1.0f);
    f->glDrawArrays(GL_LINES, 0, axisVertexCount);
    axisVAO->Unbind();

    for (auto& obj : primitives) {
        //qDebug() << "draw\n";
        obj.Draw();
    }
}



void Scene::initGrid()
{
     // 1)shader
    gridShader = new Shader(":/Shaders/shaders/grid.vert",":/Shaders/shaders/grid.frag");

    // grid
    const float size = 10.0f;   //half of size (+/- size)
    const float step = 1.0f;    // step
    std::vector<GLfloat> grid;  // pos(3) + color(3) = 6 floats for vertex

    auto addLine = [&](float x1,float y1,float x2,float y2,float r,float g,float b) {
        grid.insert(grid.end(), {x1,y1,0.0f, r,g,b,  x2,y2,0.0f, r,g,b});
    };

    // thin lines
    for (float x = -size; x <= size + 1e-4f; x += step)
        addLine(x, -size, x,  size, 0.4f,0.4f,0.4f);
    for (float y = -size; y <= size + 1e-4f; y += step)
        addLine(-size, y,  size, y, 0.4f,0.4f,0.4f);
    gridVertexCount = static_cast<GLsizei>(grid.size() / 6);
    gridVAO = new VAO();
    gridVBO = new VBO(grid.data(), grid.size() * sizeof(GLfloat));
    gridVAO->Bind();
    gridVAO->linkAttribut(*gridVBO, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
    gridVAO->linkAttribut(*gridVBO, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));

    gridVAO->Unbind();
    gridVBO->Unbind();

    // 2) axes red y green x
    std::vector<GLfloat> axes;
    auto addAxis = [&](float x1,float y1,float x2,float y2,float r,float g,float b){
        axes.insert(axes.end(), {x1,y1,0.0f, r,g,b,  x2,y2,0.0f, r,g,b});
    };
    addAxis(-size, 0, size, 0, 1.0f,0.1f,0.1f); // X
    addAxis(0, -size, 0, size, 0.1f,1.0f,0.1f); // Y
    axisVertexCount = static_cast<GLsizei>(axes.size() / 6);
    axisVAO = new VAO();
    axisVBO = new VBO(axes.data(), axes.size() * sizeof(GLfloat));
    axisVAO->Bind();
    axisVAO->linkAttribut(*axisVBO, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
    axisVAO->linkAttribut(*axisVBO, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));

    axisVAO->Unbind();
    axisVBO->Unbind();
    
   
}
