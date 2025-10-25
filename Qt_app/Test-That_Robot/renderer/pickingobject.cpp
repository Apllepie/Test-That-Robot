#include "pickingobject.h"

PickingObject::PickingObject() {}

PickingObject::~PickingObject()
{

}

void PickingObject::init(GLuint w, GLuint h)
{
    initializeOpenGLFunctions();
    //create FBO
    glGenFramebuffers(1, &t_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, t_FBO);
    //create texture for prim
    glGenTextures(1, &t_picking);
    glBindTexture(GL_TEXTURE_2D, t_picking );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, w, h, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t_picking, 0);

// verify that FBO is ok
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE){
        qDebug() <<"FBO error, status: " << status << "\n";
        exit(1);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickingObject::enableWrite()
{
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, t_FBO);
}

void PickingObject::disableWrite()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

PickingObject::pixelInfo PickingObject::readPixel(unsigned int x, unsigned int y)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, t_FBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    GLuint pixel[3] = {0, 0, 0};
    glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, pixel);

    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    pixelInfo p;
    p.r = pixel[0];
    p.g = pixel[1];
    p.b = pixel[2];
    return p;
}
