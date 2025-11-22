#ifndef PATH_RENDERER_H
#define PATH_RENDERER_H

#include <QOpenGLExtraFunctions>
#include <vector>
#include <QVector3D>
#include <memory>
#include "renderer/camera.h"
#include "renderer/shaderclass.h"

class PathRenderer : protected QOpenGLExtraFunctions
{
private:
    GLuint _vao = 0;
    GLuint _vbo = 0;
    std::unique_ptr<Shader> _shader;

public:
    PathRenderer();
    ~PathRenderer();

    void init();
    void render(const std::vector<QVector3D>& path, const Camera& camera, QVector3D color);
};

#endif // PATH_RENDERER_H
