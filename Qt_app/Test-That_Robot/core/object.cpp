#include "object.h"
#include <QDebug> // Нужно для отладки

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
   _scale.setX(_scale.x()+dx);
   _scale.setY(_scale.y()+dy);
   _scale.setZ(_scale.z()+dz);
   updateModelMatrixFromParameters();
}

void Object::Scale(QVector3D factor){
   _scale = factor;
   updateModelMatrixFromParameters();
}

void Object::Rotate(float theta, bool xAxis, bool yAxis, bool zAxis)
{
    // Вращаем только если ось Z (или всегда, если у вас 2D)
    // theta - это изменение угла (дельта) в градусах
    _angle += theta;
    if(theta == 0.0f) {
        _angle = 0.0f;
    }else if(theta == 90.0f){
        _angle = 90.0f;
    }

    
    // Нормализация угла (чтобы не рос до миллиона), по желанию
    if (_angle > 360.0f) _angle -= 360.0f;
    if (_angle < -360.0f) _angle += 360.0f;

    qDebug() << "Object Rotate -> New Angle:" << _angle << "( Added:" << theta << ")";

    // Применяем угол к матрице
    updateModelMatrixFromParameters();
}

void Object::updateModelMatrixFromParameters() 
{
    _modelMatrix.setToIdentity();
    _modelMatrix.translate(_x, _y, 0);
    
    // ПОВОРОТ В ГРАДУСАХ
    _modelMatrix.rotate(_angle, 0, 0, 1);
    
    _modelMatrix.scale(_scale);
}

void Object::updateParametersFromModelMatrix() {
    // !!! ВАЖНО !!!
    // ОСТАВЬТЕ ЭТУ ФУНКЦИЮ ПУСТОЙ ИЛИ ТОЛЬКО ДЛЯ ПОЗИЦИИ.
    // Попытка извлечь _angle из матрицы (через кватернионы или acos)
    // ЛОМАЕТСЯ на объектах с неравномерным масштабом (стены/коробки).
    
    _x = _modelMatrix.column(3).x();
    _y = _modelMatrix.column(3).y();
    
    // _angle НЕ ТРОГАЕМ! Он хранится в переменной.
    
    _scale = _modelMatrix.column(0).toVector3D();
}