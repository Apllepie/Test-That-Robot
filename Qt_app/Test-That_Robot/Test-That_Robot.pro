QT       += core gui opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

INCLUDEPATH += \
    $$PWD/app \
    $$PWD/core \
    $$PWD/renderer \
    $$PWD/renderer/opengl_wrappers


HEADERS += \
    # ---  (App) ---
    app/mainwindow.h \
    app/opengl.h \
    app/inputcontroller.h \
    \
    # ---  (Core) ---
    core/world.h \
    core/object.h \
    core/robot.h \
    core/occupancy_grid.h \
    \
    # ---  (Renderer) ---
    renderer/scene.h \
    renderer/camera.h \
    renderer/mesh.h \
    renderer/shaderclass.h \
    renderer/pickingobject.h \
    renderer/opengl_wrappers/vao.h \
    renderer/opengl_wrappers/vbo.h \
    renderer/opengl_wrappers/ebo.h


SOURCES += \
    # ---  (App) ---
    app/main.cpp \
    app/mainwindow.cpp \
    app/opengl.cpp \
    app/inputcontroller.cpp \
    \
    # ---  (Core) ---
    core/world.cpp \
    core/object.cpp \
    core/robot.cpp \
    core/occupancy_grid.cpp \
    \
    # ---  (Renderer) ---
    renderer/scene.cpp \
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
