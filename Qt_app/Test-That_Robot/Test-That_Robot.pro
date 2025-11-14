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
    $$PWD/renderer/opengl_wrappers \
    $$PWD/scripting


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
    renderer/gizmohandler.h \
    renderer/grid_renderer.h \
    renderer/scene_renderer.h \
    renderer/camera.h \
    renderer/mesh.h \
    renderer/shaderclass.h \
    renderer/pickingobject.h \
    renderer/opengl_wrappers/vao.h \
    renderer/opengl_wrappers/vbo.h \
    renderer/opengl_wrappers/ebo.h \
    \
    scripting/luasyntaxhighlighter.h \
    scripting/scriptapibridge.h \
    scripting/scriptingmanager.h

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
    renderer/gizmohandler.cpp \
    renderer/grid_renderer.cpp \
    renderer/scene_renderer.cpp \
    renderer/camera.cpp \
    renderer/mesh.cpp \
    renderer/shaderclass.cpp \
    renderer/pickingobject.cpp \
    renderer/opengl_wrappers/vao.cpp \
    renderer/opengl_wrappers/vbo.cpp \
    renderer/opengl_wrappers/ebo.cpp \
    \
    scripting/luasyntaxhighlighter.cpp \
    scripting/scriptapibridge.cpp \
    scripting/scriptingmanager.cpp

FORMS += \
    app/mainwindow.ui

# path to lua and sol2
LUA_PATH = $$PWD/3rdparty/lua-5.4.8/src
SOL2_PATH = $$PWD/3rdparty/sol2-develop

# path to inc files 
INCLUDEPATH += \
    $$LUA_PATH \
    $$SOL2_PATH/include

# src files
SOURCES += \
     $$LUA_PATH/lapi.c \
    $$LUA_PATH/lauxlib.c \
    $$LUA_PATH/lbaselib.c \
    $$LUA_PATH/lcode.c \
    $$LUA_PATH/lcorolib.c \
    $$LUA_PATH/lctype.c \
    $$LUA_PATH/ldblib.c \
    $$LUA_PATH/ldebug.c \
    $$LUA_PATH/ldo.c \
    $$LUA_PATH/ldump.c \
    $$LUA_PATH/lfunc.c \
    $$LUA_PATH/lgc.c \
    $$LUA_PATH/linit.c \
    $$LUA_PATH/liolib.c \
    $$LUA_PATH/llex.c \
    $$LUA_PATH/lmathlib.c \
    $$LUA_PATH/lmem.c \
    $$LUA_PATH/loadlib.c \
    $$LUA_PATH/lobject.c \
    $$LUA_PATH/lopcodes.c \
    $$LUA_PATH/loslib.c \
    $$LUA_PATH/lparser.c \
    $$LUA_PATH/lstate.c \
    $$LUA_PATH/lstring.c \
    $$LUA_PATH/lstrlib.c \
    $$LUA_PATH/ltable.c \
    $$LUA_PATH/ltablib.c \
    $$LUA_PATH/ltm.c \
 #   $$LUA_PATH/lua.c \
 #   $$LUA_PATH/luac.c \
    $$LUA_PATH/lundump.c \
    $$LUA_PATH/lutf8lib.c \
    $$LUA_PATH/lvm.c \
    $$LUA_PATH/lzio.c



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
