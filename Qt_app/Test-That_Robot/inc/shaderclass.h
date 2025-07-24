#ifndef SHADERCLASS_H
#define SHADERCLASS_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <string>
#include <QFile>
#include <QTextStream>
#include <cerrno>

using namespace std;

string get_file_contents(const char * filename);

class Shader
{
public:
    Shader();
    ~Shader();
    Shader(const char* vertexFile, const char * fragmentFile);
    GLuint ID;

    void Activate();
    void Delete();

private:
    QOpenGLExtraFunctions *f;
};

#endif // SHADERCLASS_H
