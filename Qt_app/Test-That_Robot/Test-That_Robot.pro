QT       += core gui opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# QMAKE_CXXFLAGS += -fsanitize=address
# QMAKE_LFLAGS += -fsanitize=address

INCLUDEPATH += \
    $$PWD/app \
    $$PWD/core \
    $$PWD/renderer \
    $$PWD/renderer/opengl_wrappers


HEADERS += \
    app/mainwindow.h \
    app/openglwidget.h \
    app/inputcontroller.h \
    \
    core/world.h \
    core/object.h \
    core/robot.h \
    core/occupancy_grid.h \
    core/obstacle.h \
    \
    renderer/scene_renderer.h \
    renderer/camera.h \
    renderer/mesh.h \
    renderer/shaderclass.h \
    renderer/pickingobject.h \
    renderer/opengl_wrappers/vao.h \
    renderer/opengl_wrappers/vbo.h \
    renderer/opengl_wrappers/ebo.h

SOURCES += \
    app/main.cpp \
    app/mainwindow.cpp \
    app/openglwidget.cpp \
    app/inputcontroller.cpp \
    \
    core/world.cpp \
    core/object.cpp \
    core/robot.cpp \
    core/occupancy_grid.cpp \
    core/obstacle.cpp \
    \
    renderer/scene_renderer.cpp \
    renderer/camera.cpp \
    renderer/mesh.cpp \
    renderer/shaderclass.cpp \
    renderer/pickingobject.cpp \
    renderer/opengl_wrappers/vao.cpp \
    renderer/opengl_wrappers/vbo.cpp \
    renderer/opengl_wrappers/ebo.cpp
FORMS += \
    app/mainwindow.ui


TRANSLATIONS += \
    Test-That_Robot_pl_PL.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc\
