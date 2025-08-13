#ifndef OBJECT_H
#define OBJECT_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>
#include <QMatrix4x4>

#include "shaderclass.h"
#include "vbo.h"
#include "vao.h"
#include "ebo.h"

struct object_buff
{
    VBO *vbo;
    VAO *vao;
    EBO *ebo;
};

class Object
{
public:
    enum class type {BOX, TRIANGLE};

    Object();
    Object(std::vector<GLfloat> vertices, std::vector<GLuint> indices, std::vector<GLfloat> Color );
    Object(type TYPE, std::vector<GLfloat> Color);

    ~Object();

    void Destruct();


    QMatrix4x4 modelMatrix;
    QVector3D pickcolor  {1.0f, 1.0f, 1.0f};
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::vector<GLfloat> Color;


    //fun
    void initialize();
    void initialize(object_buff buff);
    void addModel(GLuint &uniID);
    void Draw(Shader *shader);
    object_buff getBuff();
    void printVertices();

private:
    QOpenGLExtraFunctions *f;

    VBO *vbo;
    VAO *vao;
    EBO *ebo;
    GLuint uniID;

    void Box();
    void Triangle();
    void addColor();

};

#endif // OBJECT_H
