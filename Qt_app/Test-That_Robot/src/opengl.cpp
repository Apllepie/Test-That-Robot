#include "opengl.h"


#include "opengl.h"
#include <GL/gl.h>
#include <QOpenGLContext>
#include <QDebug>
#include <math.h>

OpenGl::OpenGl(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

OpenGl::~OpenGl()
{
    shader.Delete();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);


}

void OpenGl::initializeGL()
{
    initializeOpenGLFunctions();
    //Vertices coordinates
    GLfloat vertices[] =
        {
            0.0f, float(0.5*sqrt(3)/3), 0.0f, // upper
            0.5f/2.0, float(-0.5*sqrt(3)/6), 0.0f, // lower right
            -0.5f/2.0f, float(-0.5*sqrt(3)/6), 0.0f, // lower left

            0.0f, float(-0.5 * sqrt(3)/6), 0.0f, // inner lower
            -0.5f/4.0f, float(0.5*sqrt(3)/12), 0.0f, // inner lower left
            0.5f/4.0f, float(0.5*sqrt(3)/12), 0.0f // inner lower right
        };
        
    GLuint indices[] = { //indices should start from 0
        2,4,3, // lower left triangle
        3,5,1, // lower right triangle
        4,0,5 // upper triangle
        //5,6,4 // inner triangle
    };    

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    qDebug() << "OpenGL initialized";

    shader = Shader(":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");

     qDebug() << "Shader initialized";
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGl::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
}

void OpenGl::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    shader.Activate();
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
}
