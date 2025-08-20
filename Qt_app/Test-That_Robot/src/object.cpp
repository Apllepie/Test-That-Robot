#include "object.h"



Object::Object()
{

}

Object::Object(Mesh *mesh):mesh{mesh}
{
    scaleMatrix.setToIdentity();
    ZrotateMatrix.setToIdentity();
    YrotateMatrix.setToIdentity();
    XrotateMatrix.setToIdentity();
    transMatrix.setToIdentity();

    modelMatrix = MakeModelMatrix();
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

void Object::Translate(float x, float y, float z)
{
    transMatrix.translate(x, y, z);
    modelMatrix = MakeModelMatrix();
}

void Object::Scale(float x, float y, float z)
{
    if(x < 0 ){
        x +=0.4f;
        y +=0.4f;
        z +=0.4f;
    }
    qDebug()<<"x = "<< x <<" y = "<< y<<" z = "<< z <<"\n";

    scaleMatrix.scale(x, y, z);

    modelMatrix = MakeModelMatrix();
}

void Object::RotateZ(float theta)
{
    ZrotateMatrix.rotate(theta, 0,0,1);
    modelMatrix = MakeModelMatrix();
}

void Object::RotateY(float theta)
{
    YrotateMatrix.rotate(theta, 0,1,0);
    modelMatrix = MakeModelMatrix();
}

void Object::RotateX(float theta)
{
    XrotateMatrix.rotate(theta, 1,0,0);
    modelMatrix = MakeModelMatrix();
}

QMatrix4x4 Object::MakeModelMatrix()
{
    return  transMatrix * XrotateMatrix * YrotateMatrix * ZrotateMatrix * scaleMatrix;

}








