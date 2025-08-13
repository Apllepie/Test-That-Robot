#include "scene.h"

Scene::Scene() {}

Scene::~Scene()
{
    for (auto it : primitives){
        it.Destruct();
    }

    if(defaultShader){defaultShader->Delete(); delete defaultShader; defaultShader = nullptr; }
    if(frameShader){frameShader->Delete(); delete frameShader; frameShader = nullptr;}
    if (gridShader) { gridShader->Delete(); delete gridShader ;gridShader = nullptr; }
    if (gridVBO) {gridVBO->Delete(); delete gridVBO; gridVBO = nullptr;}
    if (gridVAO) {gridVAO->Delete(); delete gridVAO; gridVAO = nullptr;}
    if (axisVBO) {axisVBO->Delete(); delete axisVBO; axisVBO = nullptr;}
    if (axisVAO) {axisVAO->Delete(); delete axisVAO; axisVAO = nullptr;}
}

void Scene::initialize()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    defaultShader = new Shader (":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");
    frameShader = new Shader(":/Shaders/shaders/pick.vert",":/Shaders/shaders/pick.frag");


    primitives.emplace_back(Object(Object::type::BOX, {1.0f, 1.0f, 1.0f}));
    primitives.emplace_back(Object(Object::type::BOX, {1.0f, 0.0f, 1.0f}));
    qDebug() <<"objects = " <<primitives.size()<<"\n";
    primitives[0].initialize();
    primitives[1].initialize(primitives[0].getBuff());
    //primitives[0].printVertices();
    primitives[1].modelMatrix.translate(1.0f, 2.0f, 0.0f);
    initGrid();

}

void Scene::resize(int w, int h)
{
    this->w = w;
    this->h = h;
    picking.init(w,h);
    f->glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
    //camera.changeProjection(w, h, 45.0f, 0.1f, 100.0f);
}

void Scene::paint(Camera& camera)
{
    picking.enableWrite();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintPicking(camera);
    picking.disableWrite();

    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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



    for (size_t i = 0; i < primitives.size(); ++i) {
        defaultShader->Activate();
        camera.Activate(defaultShader);

        bool isSelected = (static_cast<int>(i) == selectedObjectIndex);
        GLint loc = f->glGetUniformLocation(defaultShader->ID, "isSelected");
        f->glUniform1i(loc, isSelected ? 1 : 0);

        primitives[i].Draw(defaultShader);
    }
}

void Scene::paintPicking(Camera &camera)
{
    GLuint id =0;

    for(auto& obj :primitives){
        frameShader->Activate();
        camera.Activate(frameShader);

        GLint loc = f->glGetUniformLocation(frameShader->ID, "objectID");
        GLint mvp = f->glGetUniformLocation(frameShader->ID, "model");
        f->glUniform1ui(loc, id+1);
        f->glUniformMatrix4fv(mvp,1, GL_FALSE, obj.modelMatrix.constData());


        obj.Draw(frameShader);
        id++;
    }
}

void Scene::selectObject(int index)
{
    if (index >= 0 && index < static_cast<int>(primitives.size())) {
        selectedObjectIndex = index;
    } else {
        selectedObjectIndex = -1;
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
