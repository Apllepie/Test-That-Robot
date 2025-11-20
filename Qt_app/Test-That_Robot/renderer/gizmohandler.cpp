#include "gizmohandler.h"

void GizmoHandler::createGizmoHandles()
{
        _gizmo.clear();
        // Локальные позиции для 8 хэндлов вокруг объекта (предполагаем, что объект - квадрат от -1 до 1)
        // Эти позиции будут автоматически масштабироваться и поворачиваться вместе с объектом.
        _gizmo.push_back({QVector3D( 1.0f,  1.0f, 1.0f), GizmoAxis::None, 0}); // Центр
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

    // Получаем направления осей объекта в мировом пространстве из матрицы модели.
    // Они уже учитывают поворот.
    QVector3D xAxis = QVector3D(_m.column(0)).normalized();
    QVector3D yAxis = QVector3D(_m.column(1)).normalized();

    // Коэффициент смещения. Можете его настроить.
    float offset = HANDL_SIZE * 0.75f;

    for(size_t i = 1; i < _gizmo.size(); i++){
        const auto& handle = _gizmo[i];

        // 1. Получаем позицию края/угла в мировом пространстве.
        QVector3D edgeWorldPos = _m.map(handle.localPosition);

        // 2. Определяем направление смещения в мировом пространстве.
        QVector3D offsetDirection;
        switch (handle.pickingId) {
        case 1: offsetDirection = xAxis; break;             // Правый
        case 2: offsetDirection = -xAxis; break;            // Левый
        case 3: offsetDirection = yAxis; break;             // Верхний
        case 4: offsetDirection = -yAxis; break;            // Нижний
        case 5: offsetDirection = (xAxis + yAxis).normalized(); break;  // Верхний правый
        case 6: offsetDirection = (-xAxis + yAxis).normalized(); break; // Верхний левый
        case 7: offsetDirection = (-xAxis - yAxis).normalized(); break; // Нижний левый
        case 8: offsetDirection = (xAxis - yAxis).normalized(); break;  // Нижний правый
        }

        // 3. Вычисляем финальную позицию хэндла.
        QVector3D finalGizmoPos = edgeWorldPos + offsetDirection * offset;

        // 4. Рисуем хэндл в этой позиции.
        GLuint locN = glGetUniformLocation(_s->ID, "model");
        QMatrix4x4 newMod;
        newMod.setToIdentity();
        newMod.translate(finalGizmoPos);
        glUniformMatrix4fv(locN, 1, GL_FALSE, newMod.constData());
        _mesh->Draw();
    }
}

void GizmoHandler::drawGizmoForPicking(Shader *_s, QMatrix4x4 _m, uint _bId)
{
    if(!_mesh || !_s){
        return;
    }

    GLuint modelLoc = glGetUniformLocation(_s->ID, "model");
    GLuint handleIdLoc = glGetUniformLocation(_s->ID, "handleID");
    GLuint idLoc = glGetUniformLocation(_s->ID, "objectID");

    for (const auto& handle : _gizmo) {

        QVector3D handleWorldPos = _m.map(handle.localPosition);
        QMatrix4x4 handleModelMatrix;
        handleModelMatrix.setToIdentity();
        if(handle.pickingId == 0 ){
            handleModelMatrix = _m;
            handleModelMatrix.scale(8);
        }
        else{
            handleModelMatrix.translate(handleWorldPos);}
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, handleModelMatrix.constData());
        glUniform1ui(idLoc, _bId);
        glUniform1ui(handleIdLoc, handle.pickingId);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, handleModelMatrix.constData());

        _mesh->Draw();
    }
}
