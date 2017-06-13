#-------------------------------------------------
#
# Project created by QtCreator 2017-05-24T21:31:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SdlPlay
TEMPLATE = lib
CONFIG += staticlib
DEFINES += SDLPLAY_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 输出YUV格式的视频，测试用
#DEFINES +=  OUT_FILE_YUV_E

CONFIG  +=  c++11

unix{
DESTDIR +=  $$PWD/../../bin/
}

win32{
CONFIG(debug, debug | release):{
DESTDIR +=  $$PWD/../../../bin/Debug/Plugins/VideoDisplay/SdlPlay/
}else{
DESTDIR +=  $$PWD/../../../bin/Release/Plugins/VideoDisplay/SdlPlay/
}
}

INCLUDEPATH +=  . \
                ./../../ \
                ./../../../Streamer

SOURCES +=  SdlPlay.cpp

HEADERS +=  SdlPlay.h

# 引入SDL库
INCLUDEPATH +=  $$PWD/../../../lib/SDL/include
LIBS    +=  -L$$PWD/../../../lib/SDL/lib/   -lSDL \
                                            -lSDLmain


unix {
    target.path = /usr/lib
    INSTALLS += target
}
