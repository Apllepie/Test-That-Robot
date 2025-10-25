#include "shaderclass.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <cstring>

string get_file_contents(const char * filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error(std::string("Cannot open file: ") + filename);
    }

    QTextStream in(&file);
    QString content = in.readAll();
    return content.toStdString();
}


Shader::Shader()
{

}

Shader::~Shader()
{

}

Shader::Shader(const char* vertexFile, const char * fragmentFile)
{
    initializeOpenGLFunctions();

    string vertexCode = get_file_contents(vertexFile);
    string fragmentCode = get_file_contents(fragmentFile);

    const char * vertexSource = vertexCode.c_str();
    const char * fragmentSource = fragmentCode.c_str();

    //create vertex shader and get refference
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //attached shader source to shader object
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    //compile shader into mashine code
    glCompileShader(vertexShader);
    compileError(vertexShader,"VERTEX");

    //create fragment shader and get reference
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //attached shader source to shader object
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    //compile shader into mashine code
    glCompileShader(fragmentShader);
    compileError(fragmentShader,"FRAGMENT");

    //create shaderProgram object
    ID = glCreateProgram();
    //attached fragmentShader and Vertex Shader to shaderProgram
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);

    //wrap up/ links all shaders into the shader program
    glLinkProgram(ID);
    compileError(ID, "PROGRAM");

    //delete now useless vertex and fragment shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
     glDeleteProgram(ID);
}

void Shader::compileError(unsigned int handle, const char* type)
{
    GLint ok = GL_FALSE;
    char log[4096] = {0};

    if (std::strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(handle, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glGetShaderInfoLog(handle, sizeof(log), nullptr, log);
            qDebug() << "SHADER COMPILATION ERROR (" << type << "):\n" << log;
        }
    } else {
        glGetProgramiv(handle, GL_LINK_STATUS, &ok);
        if (!ok) {
            glGetProgramInfoLog(handle, sizeof(log), nullptr, log);
            qDebug() << "SHADER LINKING ERROR (PROGRAM):\n" << log;
        }
    }
}



