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
    Shader *shader;
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
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::vector<GLfloat> Color;

    //fun
    void initialize(const char* vertexFile, const char * fragmentFile);
    void initialize(object_buff buff);
    void addModel(GLuint &uniID);
    void Draw();
    object_buff getBuff();
    void printVertices();

private:
    QOpenGLExtraFunctions *f;

    Shader *shader;
    VBO *vbo;
    VAO *vao;
    EBO *ebo;
    GLuint uniID;

    void Box();
    void Triangle();
    void addColor();

};

#endif // OBJECT_H
