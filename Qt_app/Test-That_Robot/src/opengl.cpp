#include "opengl.h"


#include "opengl.h"
#include <GL/gl.h>
#include <QOpenGLContext>
#include <QDebug>
#include <math.h>

OpenGl::OpenGl(QWidget *parent): QOpenGLWidget(parent)
{

}

OpenGl::~OpenGl()
{
    shader->Delete();
    vbo1->Delete();
    vao1->Delete();
    ebo1->Delete();

    delete    shader;
    delete    vbo1;
    delete    vao1;
    delete    ebo1;

}

void OpenGl::initializeGL()
{
    initializeOpenGLFunctions();
    //Vertices coordinates
    // GLfloat vertices[] =
    //     {   // point                                           // color
    //         0.0f, float(0.5*sqrt(3)/3), 0.0f,                  0.0f, 1.0f, 0.0f, // upper
    //         0.5f/2.0, float(-0.5*sqrt(3)/6), 0.0f,             0.0f, 0.0f, 1.0f, // lower right
    //         -0.5f/2.0f, float(-0.5*sqrt(3)/6), 0.0f,           1.0f, 0.0f, 0.0f,// lower left

    //         0.0f, float(-0.5 * sqrt(3)/6), 0.0f,               0.5f, 0.0f, 0.5f,// inner lower
    //         -0.5f/4.0f, float(0.5*sqrt(3)/12), 0.0f,           0.5f, 0.5f, 0.0f,// inner lower left
    //         0.5f/4.0f, float(0.5*sqrt(3)/12), 0.0f,            0.0f, 0.5f, 0.5f// inner lower right
    //     };
    // GLuint indices[] = { //indices should start from 0
    //     2,4,3, // lower left triangle
    //     3,5,1, // lower right triangle
    //     4,0,5 // upper triangle
    //     //5,6,4 // inner triangle
    // };
    GLfloat vertices[] =
        {   // point                               // color
            -0.5f, -0.5f, 0.0f,                     0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.0f,                      1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.0,                        0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.0,                       0.0f, 0.0f, 1.0f
        };
        
    GLuint indices[] = { //indices should start from 0
        0,2,1,
        0,2,3
    };    

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    qDebug() << "OpenGL initialized";

    camera.Init();

    shader = new Shader(":/Shaders/shaders/default.vert",":/Shaders/shaders/default.frag");

     qDebug() << "Shader initialized";
     
    vao1 = new VAO;
    vbo1 = new VBO(vertices, sizeof(vertices));
    ebo1 = new EBO(indices, sizeof(indices));

    vao1->Bind();
    vao1->linkAttribut(*vbo1, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0 );
    vao1->linkAttribut(*vbo1, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3*sizeof(float)) );
    ebo1->Bind();

    vao1->Unbind();
    vbo1->Unbind();
    ebo1->Unbind();

    uniID =  glGetUniformLocation(shader->ID, "model");
   

}

void OpenGl::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    qDebug() << "Resized to" << w << "x" << h;
    //camera.changeProjection(w, h, 45.0f, 0.1f, 100.0f);
}
void OpenGl::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    shader->Activate();
    QMatrix4x4 model;
    model.setToIdentity();
    glUniformMatrix4fv(uniID, 1, GL_FALSE, model.constData());
    camera.Activate(shader);
    vao1->Bind();
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    GLenum err;
while ((err = glGetError()) != GL_NO_ERROR) {
    qDebug() << "OpenGL error:" << err;
}
}

void OpenGl::wheelEvent(QWheelEvent *event)
{
        float delta = event->angleDelta().y() / 120.0f;
        camera.moveCloser_Away(-delta);
        update();
}

void OpenGl::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        rightMousePress = true;
        lastMousePos  = event->pos();
    }
    if(event->button() == Qt::LeftButton){
        leftMousePress = true;
    }
}

void OpenGl::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        rightMousePress = false;
    }
    if(event->button() == Qt::LeftButton){
        leftMousePress = false;
    }
}

void OpenGl::mouseMoveEvent(QMouseEvent *event)
{
    if(rightMousePress){
        QPoint delta = event->pos() - lastMousePos;
        lastMousePos = event->pos();
        camera.Move(delta);
    }

    update();
}
