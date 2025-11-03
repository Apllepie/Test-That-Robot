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
    updateModelMatrixFromParameters();
}
void Object::Translate(QVector3D dir)
{
    _x = dir.x();
    _y = dir.y();
    updateModelMatrixFromParameters();
}

void Object::Scale(float dx, float dy, float dz)
{
    if(_scale.x() * dx <= 0.0f) dx = 0.01f;
    if(_scale.y() * dy <= 0.0f) dy = 0.01f;
    if(_scale.z() * dz <= 0.0f) dz = 0.01f;
   _scale.setX(_scale.x()*dx);
   _scale.setY(_scale.y()*dy);
    _scale.setZ(_scale.z()*dz);
    updateModelMatrixFromParameters();
}
 void Object::Scale(QVector3D factor){
   _scale = factor;
    updateModelMatrixFromParameters();
 }

void Object::Rotate(float theta, bool xAxis, bool yAxis, bool zAxis)
{
    if(xAxis)
        _modelMatrix.rotate(theta, 1,0,0);
    if(yAxis)
        _modelMatrix.rotate(theta, 0,1,0);
    if(zAxis)
        _modelMatrix.rotate(theta, 0,0,1);
    updateParametersFromModelMatrix();

}

    void Object::updateModelMatrixFromParameters() 
    {
        _modelMatrix.setToIdentity();
        _modelMatrix.translate(_x, _y, 0);
        _modelMatrix.rotate(_angle, 0, 0, 1);
        _modelMatrix.scale(_scale);
    }
    void Object::updateParametersFromModelMatrix() {
        _x = _modelMatrix.column(3).x();
        _y = _modelMatrix.column(3).y();
        QQuaternion q = QQuaternion::fromRotationMatrix(_modelMatrix.toGenericMatrix<3,3>());
        _angle = q.toEulerAngles().z();

        _scale = _modelMatrix.column(0).toVector3D(); // Assuming uniform scaling

    }








