#ifndef OPENGL_H
#define OPENGL_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <QDebug>
#include <math.h>

#include <QMatrix4x4>
#include <QVector3D>
#include <QTime>
#include <QMouseEvent>

#include "shaderclass.h"

class OpenGl : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit OpenGl(QWidget *parent = nullptr);
    ~OpenGl();

protected:
    void initializeGL() ;
    void resizeGL(int w, int h) ;
    void paintGL() ;

    Shader shader;

   GLuint shaderProgram;
   GLuint VBO, VAO, EBO; // containers for vertex buffer obgect and vertex array objeect

};

#endif // OPENGL_H

