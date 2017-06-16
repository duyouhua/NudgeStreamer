#-------------------------------------------------
#
# Project created by QtCreator 2017-05-15T09:02:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Streamer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG  +=  c++11 thread

unix{
DESTDIR +=  $$PWD/../bin/
}

win32{
CONFIG(debug, debug | release):{
DESTDIR +=  $$PWD/../bin/Debug/
#system(Copy2Debug.bat)
}else{
DESTDIR +=  $$PWD/../bin/Release/
#system(Copy2Release.bat)
}
}

INCLUDEPATH +=  . \
                ./../Plugins

SOURCES +=  main.cpp \
            Streamer.cpp \
            ./Factory/InterceptFactory.cpp \
            ./Factory/NetworkFactory.cpp \
            ./Factory/DisplayFactory.cpp \
            ./Factory/EncodeFactory.cpp \
            ./Factory/PushFactory.cpp

HEADERS +=  Streamer.h \
            DataToUI.h \
            ./Factory/InterceptFactory.h \
            ./Factory/NetworkFactory.h \
            ./Factory/DisplayFactory.h \
            ./Factory/EncodeFactory.h \
            ./Factory/PushFactory.h \
            ./../Plugins/Frame.h \
            ./../Plugins/PicYUV.h \
            ./../Plugins/NetworkParament.h \
            ./../Plugins/NetworkQuality.h \
            ./../Plugins/VideoFileOut.h \
            ./../Plugins/DataBufferPointer.h \
            ./../Plugins/DataBufferQueue.h \
            ./../Plugins/InterceptScreen.h \
            ./../Plugins/PushFlow.h \
            ./../Plugins/VideoDisplay.h \
            ./../Plugins/VideoEncode.h \
            ./../Plugins/LogMessage.h

FORMS    += Streamer.ui

#引入ffmpeg库
INCLUDEPATH +=  $$PWD/../lib/ffmpeg/include
LIBS    +=  -L$$PWD/../lib/ffmpeg/lib/  -lavcodec \
                                        -lavdevice \
                                        -lavfilter \
                                        -lavformat \
                                        -lavutil \
                                        -lpostproc \
                                        -lswscale

## 引入SDL库
#INCLUDEPATH +=  $$PWD/../lib/SDL/include
#LIBS    +=  -L$$PWD/../lib/SDL/lib/ -lSDL \
#                                    -lSDLmain


# 引入ws_32库
LIBS    +=  -lWS2_32

# 引入rtmp库
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/rtmp/lib/Release/ -lrtmp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/rtmp/lib/Debug/ -lrtmp
else:unix: LIBS += -L$$PWD/../lib/rtmp/lib/ -lrtmp

INCLUDEPATH += $$PWD/../lib/rtmp/include
DEPENDPATH += $$PWD/../lib/rtmp/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/rtmp/lib/Release/librtmp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/rtmp/lib/Debug/librtmp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/rtmp/lib/Release/rtmp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/rtmp/lib/Debug/rtmp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../lib/rtmp/lib/librtmp.a

# Log
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/Release/ -lLog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/Debug/ -lLog
else:unix: LIBS += -L$$PWD/../bin/ -lLog

INCLUDEPATH += $$PWD/../Log
DEPENDPATH += $$PWD/../Log

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/libLog.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/libLog.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Log.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Log.lib
else:unix: PRE_TARGETDEPS += $$PWD/../bin/libLog.a

# GdiGrab
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/Release/Plugins/InterceptScreen/GdiGrab/ -lGdiGrab
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/Debug/Plugins/InterceptScreen/GdiGrab/ -lGdiGrab
else:unix: LIBS += -L$$PWD/../bin/ -lGdiGrab

INCLUDEPATH += $$PWD/../Plugins/InterceptScreen/GdiGrab
DEPENDPATH += $$PWD/../Plugins/InterceptScreen/GdiGrab

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/InterceptScreen/GdiGrab/libGdiGrab.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/.../bin/Debug/Plugins/InterceptScreen/GdiGrab/libGdiGrab.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/InterceptScreen/GdiGrab/GdiGrab.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/InterceptScreen/GdiGrab/GdiGrab.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../bin/libGdiGrab.a

# TcpTest
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/Release/Plugins/NetworkQuality/TcpTest/ -lTcpTest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/Debug/Plugins/NetworkQuality/TcpTest/ -lTcpTest
else:unix: LIBS += -L$$PWD/../bin/ -lTcpTest

INCLUDEPATH += $$PWD/../Plugins/NetworkQuality/TcpTest
DEPENDPATH += $$PWD/../Plugins/NetworkQuality/TcpTest

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/NetworkQuality/TcpTest/libTcpTest.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/NetworkQuality/TcpTest/libTcpTest.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/NetworkQuality/TcpTest/TcpTest.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/NetworkQuality/TcpTest/TcpTest.lib
else:unix: PRE_TARGETDEPS += $$PWD/../bin/libTcpTest.a

# PicSendUI
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/Release/Plugins/VideoDisplay/PicSendUI/ -lPicSendUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/Debug/Plugins/VideoDisplay/PicSendUI/ -lPicSendUI
else:unix: LIBS += -L$$OUT_PWD/../bin/ -lPicSendUI

INCLUDEPATH += $$PWD/../Plugins/VideoDisplay/PicSendUI
DEPENDPATH += $$PWD/../Plugins/VideoDisplay/PicSendUI

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/VideoDisplay/PicSendUI/libPicSendUI.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/VideoDisplay/PicSendUI/libPicSendUI.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/VideoDisplay/PicSendUI/PicSendUI.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/VideoDisplay/PicSendUI/PicSendUI.lib
else:unix: PRE_TARGETDEPS += $$PWD/../bin/libPicSendUI.a

## SDL
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/Release/Plugins/VideoDisplay/SdlPlay/ -lSdlPlay
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/Debug/Plugins/VideoDisplay/SdlPlay/ -lSdlPlay
#else:unix: LIBS += -L$$PWD/../bin/ -lSdlPlay

#INCLUDEPATH += $$PWD/../Plugins/VideoDisplay/SdlPlay
#DEPENDPATH += $$PWD/../Plugins/VideoDisplay/SdlPlay

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/VideoDisplay/SdlPlay/libSdlPlay.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/VideoDisplay/SdlPlay/libSdlPlay.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/VideoDisplay/SdlPlay/SdlPlay.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/VideoDisplay/SdlPlay/SdlPlay.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../bin/libSdlPlay.a

# AvEncode
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/Release/Plugins/VideoEncode/AvEncode/ -lAvEncode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/Debug/Plugins/VideoEncode/AvEncode/ -lAvEncode
else:unix: LIBS += -L$$PWD/../bin/ -lAvEncode

INCLUDEPATH += $$PWD/../Plugins/VideoEncode/AvEncode
DEPENDPATH += $$PWD/../Plugins/VideoEncode/AvEncode

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/VideoEncode/AvEncode/libAvEncode.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/VideoEncode/AvEncode/libAvEncode.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/VideoEncode/AvEncode/AvEncode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/VideoEncode/AvEncode/AvEncode.lib
else:unix: PRE_TARGETDEPS += $$PWD/../bin/libAvEncode.a

# RtmpPush
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/Release/Plugins/PushFlow/RtmpPush/ -lRtmpPush
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/Debug/Plugins/PushFlow/RtmpPush/ -lRtmpPush
else:unix: LIBS += -L$$PWD/../bin/ -lRtmpPush

INCLUDEPATH += $$PWD/../Plugins/PushFlow/RtmpPush
DEPENDPATH += $$PWD/../Plugins/PushFlow/RtmpPush

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/PushFlow/RtmpPush/libRtmpPush.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/PushFlow/RtmpPush/libRtmpPush.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Release/Plugins/PushFlow/RtmpPush/RtmpPush.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/Debug/Plugins/PushFlow/RtmpPush/RtmpPush.lib
else:unix: PRE_TARGETDEPS += $$PWD/../bin/libRtmpPush.a
