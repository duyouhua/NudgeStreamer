#-------------------------------------------------
#
# Project created by QtCreator 2017-06-01T15:09:17
#
#-------------------------------------------------

QT       -= core gui

TARGET = rtmp
TEMPLATE = lib
CONFIG += staticlib
#DEFINES += RTMP_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix{
DESTDIR +=  $$PWD/lib/
}

win32{
CONFIG(debug, debug | release):{
DESTDIR +=  $$PWD/lib/Debug/
}
else{
DESTDIR +=  $$PWD/lib/Release/
}
}

INCLUDEPATH +=  ./include

SOURCES +=  ./src/amf.c \
            ./src/hashswf.c \
            ./src/log.c \
            ./src/parseurl.c \
            ./src/rtmp.c

HEADERS +=  ./include/amf.h \
            ./include/bytes.h \
            ./include/dh.h \
            ./include/dhgroups.h \
            ./include/handshake.h \
            ./include/http.h \
            ./include/log.h \
            ./include/rtmp.h \
            ./include/rtmp_sys.h

LIBS    +=  -lWS2_32

unix {
    target.path = /usr/lib
    INSTALLS += target
}
