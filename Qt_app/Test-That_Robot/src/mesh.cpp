#include "mesh.h"

Mesh::Mesh()
{

}

Mesh::Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> indices):
    vertices{vertices}, indices{indices}
{

}

Mesh::Mesh(type type, std::vector<GLfloat> color)
{
    if(type == Mesh::type::BOX){
        Box();
    }

    if(type == Mesh::type::TRIANGLE){
        Triangle();
    }

    addColor(color);
    printVertices();
}


Mesh::~Mesh()
{
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
}

void Mesh::Init()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();


    vao.Init();
    vbo.Init(vertices.data(), vertices.size()* sizeof(GLfloat));
    ebo.Init(indices.data(), indices.size()* sizeof(GLuint));

    vao.Bind();
    vao.linkAttribut(vbo, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
    vao.linkAttribut(vbo, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)));
    ebo.Bind();

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

}

void Mesh::Draw()
{
    vao.Bind();
    ebo.Bind();
    f->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::printVertices()
{
    for (size_t i = 0; i < vertices.size(); i += 6) { // step 6: 3 vertices + 3 colors
        qDebug() << "Vertex: "
                 << vertices[i] << vertices[i + 1] << vertices[i + 2]; // vertices
        qDebug() << "Color: "
                 << vertices[i + 3] << vertices[i + 4] << vertices[i + 5]; // colors
    }
}

void Mesh::Box()
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

void Mesh::Triangle()
{
    vertices = {
        0.0f, float(0.5*sqrt(3)/3), 0.0f,
        0.5f/2.0, float(-0.5*sqrt(3)/6), 0.0f,
        -0.5f/2.0f, float(-0.5*sqrt(3)/6), 0.0f
    };
    indices = {
               0,1,2};
}

void Mesh::addColor(std::vector<GLfloat> color)
{
    std::vector<GLfloat> newVertices;
    int v_value = vertices.size() / 3;
    for (int i = 0; i < v_value; i++) {
        newVertices.push_back(vertices[i * 3]);
        newVertices.push_back(vertices[i * 3 + 1]);
        newVertices.push_back(vertices[i * 3 + 2]);

        // add color
        newVertices.push_back(color[0]);
        newVertices.push_back(color[1]);
        newVertices.push_back(color[2]);
    }

    // change old vector to new
    vertices = newVertices;
}
