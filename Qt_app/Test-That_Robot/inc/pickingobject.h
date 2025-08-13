#ifndef PICKINGOBJECT_H
#define PICKINGOBJECT_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/gl.h>
#include <QOpenGLContext>
#include <cerrno>



class PickingObject
{
public:
    PickingObject();
    ~PickingObject();

    void init(GLuint w, GLuint h);
    void enableWrite();
    void disableWrite();

    struct pixelInfo
    {
        uint r;
        uint g;
        uint b;

        void print()
        {
            qDebug() << "red "<<r << "green" <<g << "blue" << b <<"\n";
        }
    };

    pixelInfo readPixel(unsigned int x, unsigned int y );

private:
    QOpenGLExtraFunctions *f;

    GLuint t_FBO;
    GLuint t_picking;
    GLuint t_depth;
};

#endif // PICKINGOBJECT_H
