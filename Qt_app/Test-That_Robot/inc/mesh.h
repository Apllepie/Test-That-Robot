#ifndef MESH_H
#define MESH_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>
#include <vector>


#include "vbo.h"
#include "vao.h"
#include "ebo.h"

class Mesh
{
public:
    enum type {BOX, TRIANGLE};
    Mesh();
    Mesh(std::vector<GLfloat> vertices, std::vector<GLuint>indices);
    Mesh(type type, std::vector<GLfloat> color);


    ~Mesh();


    //fun
    void Init();

    void Draw();

    void printVertices();

private:
    VAO vao;
    VBO vbo;
    EBO ebo;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    QOpenGLExtraFunctions* f = nullptr;

    //fun
    void Box();
    void Triangle();
    void addColor(std::vector<GLfloat> color);

};

#endif // MESH_H
