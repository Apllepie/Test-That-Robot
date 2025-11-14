#include "gizmohandler.h"

void GizmoHandler::createGizmoHandles()
{
        _gizmo.clear();
        // Локальные позиции для 8 хэндлов вокруг объекта (предполагаем, что объект - квадрат от -1 до 1)
        // Эти позиции будут автоматически масштабироваться и поворачиваться вместе с объектом.

        // Края
        _gizmo.push_back({QVector3D( 0.5f,  0.0f, 0.0f), GizmoAxis::X,  1}); // Правый
        _gizmo.push_back({QVector3D(-0.5f,  0.0f, 0.0f), GizmoAxis::X,  2}); // Левый
        _gizmo.push_back({QVector3D( 0.0f,  0.5f, 0.0f), GizmoAxis::Y,  3}); // Верхний
        _gizmo.push_back({QVector3D( 0.0f, -0.5f, 0.0f), GizmoAxis::Y,  4}); // Нижний

        // Углы
        _gizmo.push_back({QVector3D( 0.5f,  0.5f, 0.0f), GizmoAxis::XY, 5}); // Верхний правый
        _gizmo.push_back({QVector3D(-0.5f,  0.5f, 0.0f), GizmoAxis::XY, 6}); // Верхний левый
        _gizmo.push_back({QVector3D(-0.5f, -0.5f, 0.0f), GizmoAxis::XY, 7}); // Нижний левый
        _gizmo.push_back({QVector3D( 0.5f, -0.5f, 0.0f), GizmoAxis::XY, 8}); // Нижний правый
}

void GizmoHandler::createMesh()
{
    float s = HANDL_SIZE;
    _mesh = std::make_unique<Mesh>(Mesh({s, s, 0, 0, 0, 1,
                                         s, -s, 0, 0, 0, 1,
                                         -s, -s, 0, 0, 0, 1,
                                         -s , s , 0, 0, 0, 1},{0, 1, 2, 2, 0, 3}));
    _mesh->Init();
}

GizmoHandler::GizmoHandler() {
   initializeOpenGLFunctions();
    createMesh();
   createGizmoHandles();
}

void GizmoHandler::drawGizmo(Camera *_c, Shader *_s, QMatrix4x4 _m)
{
    if (!_mesh) {
        return;
    }
    _c->Activate(_s);

    for(size_t i =0 ; i < _gizmo.size(); i++){
        QVector3D gizmoLoc = _m.map( _gizmo[i].localPosition);
        GLuint locN = glGetUniformLocation(_s->ID, "model");
        QMatrix4x4 newMod;
        newMod.setToIdentity();
        newMod.translate(gizmoLoc);
        glUniformMatrix4fv(locN, 1, GL_FALSE, newMod.constData());
        _mesh->Draw();
    }
}
