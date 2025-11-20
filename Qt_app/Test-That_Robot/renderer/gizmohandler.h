#ifndef GIZMOHANDLER_H
#define GIZMOHANDLER_H

#include "camera.h"
#include "mesh.h"
#include "qopenglextrafunctions.h"
#include <QVector3D>
#include <memory>

#define HANDL_SIZE 0.06

enum GizmoAxis{
    X, Y, Z, XY, XZ, YZ, XYZ, None
};

struct GizmoHandle {
    QVector3D localPosition; // Позиция в локальных координатах объекта (например, (1, 0, 0) для правого края)
    GizmoAxis axis;          // Ось, за которую отвечает хэндл
    int pickingId;           // Уникальный ID для пикинга
};



class GizmoHandler : protected QOpenGLExtraFunctions
{
private:
    std::unique_ptr<Mesh> _mesh;
    std::vector<GizmoHandle> _gizmo;

    //function
    void createGizmoHandles();
    void createMesh();
public:
    GizmoHandler();
    void drawGizmo(Camera *_c, Shader *_s, QMatrix4x4 _m);
    void drawGizmoForPicking(Shader *_s, QMatrix4x4 _m, uint _bId);
    const GizmoHandle* getHandleInfo(int handleId) {
        for (const auto& handle : _gizmo) {
            if (handle.pickingId == handleId) {
                return &handle;
            }
        }
        return nullptr;
    }




    std::vector<GizmoHandle> getGizmo(){ return _gizmo;}
};

#endif // GIZMOHANDLER_H
