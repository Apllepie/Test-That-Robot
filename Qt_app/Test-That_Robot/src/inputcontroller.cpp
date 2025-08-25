#include "inputcontroller.h"
#include <QDebug>




void InputController::wheel(QWheelEvent *e) {
    if (!camera || !scene) return;
    float delta = e->angleDelta().y() / 120.0f;
    if (leftPressed && scene->selectedObjectIndex != -1) {
        scene->primitives[scene->selectedObjectIndex]->Scale(-delta*1.2f, -delta*1.2f, 0.0f);
    } else {
        camera->moveCloser_Away(-delta);
    }
}

void InputController::mousePress(QMouseEvent *e, float dpr, int widgetHeight) {
    if (!camera || !scene) return;

    if (e->button() == Qt::RightButton) {
        rightPressed = true;
        lastPos = e->pos();
        mouseWorldPos = getMouseWorldPos(e);
    }
    if (e->button() == Qt::LeftButton) {
        leftPressed = true;
        lastPos = e->pos();
        mouseWorldPos = getMouseWorldPos(e);
        pickAt(dpr, widgetHeight);
    }
}

void InputController::mouseRelease(QMouseEvent *e) {
    if (e->button() == Qt::RightButton) rightPressed = false;
    if (e->button() == Qt::LeftButton)  leftPressed  = false;

}

void InputController::mouseMove(QMouseEvent *e) {
    if (!camera || !scene) return;

    if (rightPressed) {
        QPoint delta = e->pos() - lastPos;
        lastPos = e->pos();
        camera->Move(delta);
    }
    if (leftPressed) {
        QPoint deltaPx = e->pos() - lastPos;
        lastPos = e->pos();
        scene->translateObject(deltaPx.x(), -deltaPx.y(), *camera);

      
    }
}

void InputController::keyPress(QKeyEvent *e) {
    if (!scene) return;
    if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete) {
        scene->deleteObject();
    }
    if(e->key() == Qt::Key_Space){
        //spacePressed = !spacePressed;
        spacePressed = true;
    }
    if(spacePressed) {scene->stopRobot();
        //qDebug() << (int)Qt::Key_Space<< "\n";
    }

    if(e->key() == Qt::Key_W){scene->startRobot(Qt::Key_W); }
    if(e->key() == Qt::Key_A){scene->startRobot(Qt::Key_A); }
    if(e->key() == Qt::Key_S){scene->startRobot(Qt::Key_S); }
    if(e->key() == Qt::Key_D){scene->startRobot(Qt::Key_D); }

}

void InputController::keyRelease(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space){
        spacePressed = false;
    }
}

QVector3D InputController::getMouseWorldPos(QMouseEvent *e) {
    if (!camera || !scene) return {};
    // предполагаем getWindowSize() возвращает QSize(int widthDevPx, int heightDevPx)
    auto winSize = scene->getWindowSize();
    float nx = (2.0f * e->pos().x()) / float(winSize.x()) - 1.0f;
    float ny = 1.0f - (2.0f * e->pos().y()) / float(winSize.y());
    QVector4D clip(nx, ny, 0.0f, 1.0f);
    QMatrix4x4 invPV = (camera->projection * camera->view).inverted();
    QVector4D world = invPV * clip;
    if (world.w() != 0.0f) world /= world.w();
    return QVector3D(world.toVector2D());
}

void InputController::pickAt(float dpr, int widgetHeight) {
    if (!scene) return;

    // widgetHeight в logical px; преобразуем как раньше:
    int px = int(lastPos.x() * dpr);
    int py = int((widgetHeight - lastPos.y()) * dpr);

    auto pixel = scene->picking.readPixel(px, py);
    GLuint pickedID = pixel.r;
    if (pickedID > 0) scene->selectObject(pickedID - 1);
    else scene->selectObject(-1);

    qDebug() << "mouse pos:" << lastPos << "pick" << pickedID;
    pixel.print();
}



