#ifndef OPENGL_H
#define OPENGL_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QTime>
#include <QMouseEvent>

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

    const char * vertexShaderSource = //vertex shader source code
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{gl_Position  = vec4(aPos.x, aPos.y, aPos.z, 1.0);}\n\0";

    const char * fragmentShaderSource = //fragment shader source code
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);}\n\0";

   GLuint shaderProgram;
   GLuint VBO, VAO, EBO; // containers for vertex buffer obgect and vertex array objeect
};

#endif // OPENGL_H

