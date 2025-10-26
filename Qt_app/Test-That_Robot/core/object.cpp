#include "object.h"





Object::Object(Mesh *mesh)
{
    this->_mesh = mesh;

    _modelMatrix.setToIdentity();
}


void Object::Translate(float dx, float dy, float dz)
{
    _x += dx;
    _y += dy;
    updateModelMatrixFromPosition();
}
void Object::Translate(QVector3D dir)
{
    _x = dir.x();
    _y = dir.y();
    updateModelMatrixFromPosition();
}

void Object::Scale(float x, float y, float z)
{
    if(x < 0 ){
        x +=0.4f;
        y +=0.4f;
        z +=0.4f;
    }
    if(_scale == 0.0f){
        _scale = 1.0f;
    }
    _scale *= x;
    updateModelMatrixFromPosition();
}

void Object::Rotate(float theta, bool xAxis, bool yAxis, bool zAxis)
{
    if(xAxis)
        _modelMatrix.rotate(theta, 1,0,0);
    if(yAxis)
        _modelMatrix.rotate(theta, 0,1,0);
    if(zAxis)
        _modelMatrix.rotate(theta, 0,0,1);
    updatePositionFromModelMatrix();

}

    void Object::updateModelMatrixFromPosition() 
    {
        _modelMatrix.setToIdentity();
        _modelMatrix.translate(_x, _y, 0);
        _modelMatrix.rotate(_angle, 0, 0, 1);
        _modelMatrix.scale(_scale, _scale, _scale);
    }
    void Object::updatePositionFromModelMatrix() {
        _x = _modelMatrix.column(3).x();
        _y = _modelMatrix.column(3).y();
        QQuaternion q = QQuaternion::fromRotationMatrix(_modelMatrix.toGenericMatrix<3,3>());
        _angle = q.toEulerAngles().z();

        QVector3D tmp;
        tmp.setX(QVector3D(_modelMatrix.column(0).toVector3D()).length());
        _scale = tmp.x();
    }








