#include "object.h"



Object::Object()
{

}

Object::Object(Mesh *mesh):mesh{mesh}
{
    scaleMatrix.scale(1,1,1);
    ZrotateMatrix.rotate(0,0,1);
    YrotateMatrix.rotate(0,1,0);
    XrotateMatrix.rotate(1,0,0);
    transMatrix.translate(0,0,0);
    modelMatrix = scaleMatrix * ZrotateMatrix * YrotateMatrix * XrotateMatrix * transMatrix;

}

Object::~Object() {
}



void Object::initialize()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();


}


void Object::addModel(GLuint &uniID)
{
    f->glUniformMatrix4fv(uniID, 1, GL_FALSE, modelMatrix.constData());
}

void Object::Draw(Shader *shader)
{
    uniID =  f->glGetUniformLocation(shader->ID, "model");

    shader->Activate();
    f->glUniformMatrix4fv(uniID, 1, GL_FALSE, modelMatrix.constData()); //shader need be activated

    mesh->Draw();
}








