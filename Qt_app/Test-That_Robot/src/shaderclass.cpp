#include "shaderclass.h"

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
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qFatal("No current OpenGL context");
    }
    f = context->extraFunctions();

    string vertexCode = get_file_contents(vertexFile);
    string fragmentCode = get_file_contents(fragmentFile);

    const char * vertexSource = vertexCode.c_str();
    const char * fragmentSource = fragmentCode.c_str();

    //create vertex shader and get refference
    GLuint vertexShader = f->glCreateShader(GL_VERTEX_SHADER);
    //attached shader source to shader object
    f->glShaderSource(vertexShader, 1, &vertexSource, NULL);
    //compile shader into mashine code
    f->glCompileShader(vertexShader);
    compileError(vertexShader,"VERTEX");

    //create fragment shader and get reference
    GLuint fragmentShader = f->glCreateShader(GL_FRAGMENT_SHADER);
    //attached shader source to shader object
    f->glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    //compile shader into mashine code
    f->glCompileShader(fragmentShader);
    compileError(fragmentShader,"FRAGMENT");

    //create shaderProgram object
    ID = f->glCreateProgram();
    //attached fragmentShader and Vertex Shader to shaderProgram
    f->glAttachShader(ID, vertexShader);
    f->glAttachShader(ID, fragmentShader);

    //wrap up/ links all shaders into the shader program
    f->glLinkProgram(ID);
    compileError(ID, "PROGRAM");

    //delete now useless vertex and fragment shader
    f->glDeleteShader(vertexShader);
    f->glDeleteShader(fragmentShader);

}

void Shader::Activate()
{
    f->glUseProgram(ID);
}

void Shader::Delete()
{
     f->glDeleteProgram(ID);
}

void Shader::compileError(unsigned int shader, const char *type)
{
    GLint hascomplete;
    char log[1024];

    if(type != "PROGRAM"){
        f->glGetShaderiv(shader, GL_COMPILE_STATUS, &hascomplete);
        if(hascomplete == GL_FALSE){
            f->glGetShaderInfoLog(shader, 1024, NULL, log);
            qDebug() <<"SHADER COMPILATION ERROR FOR: "<< type <<"\n";
        }
        }
    else
        f->glGetProgramiv(shader, GL_COMPILE_STATUS, &hascomplete);
    if(hascomplete == GL_FALSE){
        f->glGetProgramInfoLog(shader, 1024, NULL, log);
        qDebug() <<"SHADER LINKING ERROR FOR: "<< type <<"\n";
    }
}



