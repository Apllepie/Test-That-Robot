#include "object.h"

Object::Object() {}

Object::Object(std::vector<GLfloat> vertices, std::vector<GLuint> indices, std::vector<GLfloat> Color):
    vertices{vertices}, indices{indices}, Color{Color}
{

    addColor();
}

Object::Object(type TYPE,  std::vector<GLfloat> Color): Color{Color}
{
    if(TYPE == Object::type::BOX){
        Box();
    }

    if(TYPE == Object::type::TRIANGLE){
        Triangle();
    }

    addColor();
    printVertices();
}

Object::~Object()
{


}

void Object::Destruct()
{
    if(shader){
        shader->Delete();
        delete shader;
    }
    if(vbo)
    {vbo->Delete();
        delete vbo;
    }
    if(vao)
    {vao->Delete();
        delete vao;
    }
    if(ebo)
    {ebo->Delete();
        delete ebo;
    }
}

void Object::initialize(const char *vertexFile, const char *fragmentFile)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    modelMatrix.setToIdentity();

    shader = new Shader(vertexFile, fragmentFile);
    qDebug() << "Shader initialized";

    vao = new VAO;
    vbo = new VBO(vertices.data(), vertices.size() * sizeof(GLfloat));
    ebo = new EBO(indices.data(), indices.size() * sizeof(GLuint));

    vao->Bind();
    vao->linkAttribut(*vbo, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0 );
    vao->linkAttribut(*vbo, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)) );
    ebo->Bind();

    vao->Unbind();
    vbo->Unbind();
    ebo->Unbind();

    uniID =  f->glGetUniformLocation(shader->ID, "model");
}

void Object::initialize(object_buff buff)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    modelMatrix.setToIdentity();

    shader = buff.shader;
    qDebug() << "Shader initialized";

    vao = buff.vao;
    vbo = buff.vbo;
    ebo = buff.ebo;

    vao->Bind();
    vao->linkAttribut(*vbo, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0 );
    vao->linkAttribut(*vbo, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)) );
    ebo->Bind();

    vao->Unbind();
    vbo->Unbind();
    ebo->Unbind();
}

void Object::addModel(GLuint &uniID)
{
    f->glUniformMatrix4fv(uniID, 1, GL_FALSE, modelMatrix.constData());
}

void Object::Draw()
{
    shader->Activate();
    f->glUniformMatrix4fv(uniID, 1, GL_FALSE, modelMatrix.constData());
    vao->Bind();
    ebo->Bind(); // Добавьте эту строку
    f->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

object_buff Object::getBuff()
{
    return {shader, vbo, vao, ebo};
}

void Object::printVertices()
{
    for (size_t i = 0; i < vertices.size(); i += 6) { // Шаг 6: 3 координаты + 3 цвета
        qDebug() << "Vertex: "
                 << vertices[i] << vertices[i + 1] << vertices[i + 2]; // Координаты вершины
        qDebug() << "Color: "
                 << vertices[i + 3] << vertices[i + 4] << vertices[i + 5]; // Цвет вершины
    }
}

void Object::Box()
{
    vertices = {
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0,
        0.5f, -0.5f, 0.0
    };
    indices = {0,2,1,
            0,2,3};
}

void Object::Triangle()
{
    vertices = {
    0.0f, float(0.5*sqrt(3)/3), 0.0f,
    0.5f/2.0, float(-0.5*sqrt(3)/6), 0.0f,
    -0.5f/2.0f, float(-0.5*sqrt(3)/6), 0.0f
    };
    indices = {
               0,1,2};
}

void Object::addColor()
{
    std::vector<GLfloat> newVertices;
    int v_value = vertices.size() / 3;
    for (int i = 0; i < v_value; i++) {
        newVertices.push_back(vertices[i * 3]);
        newVertices.push_back(vertices[i * 3 + 1]);
        newVertices.push_back(vertices[i * 3 + 2]);

        // Добавляем цвет
        newVertices.push_back(Color[0]);
        newVertices.push_back(Color[1]);
        newVertices.push_back(Color[2]);
    }

    // Заменяем старый вектор на новый
    vertices = newVertices;
}

