QT       += core gui opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/ebo.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/opengl.cpp \
    src/shaderclass.cpp \
    src/vao.cpp \
    src/vbo.cpp

HEADERS += \
    inc/ebo.h \
    inc/mainwindow.h \
    inc/opengl.h \
    inc/shaderclass.h \
    inc/vao.h \
    inc/vbo.h

FORMS += \
    ui/mainwindow.ui

TRANSLATIONS += \
    Test-That_Robot_pl_PL.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource/resources.qrc
