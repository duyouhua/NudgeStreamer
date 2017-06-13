#-------------------------------------------------
#
# Project created by QtCreator 2017-5-14T00:24:37
#
#-------------------------------------------------

#表示这个是Qt跨目录,由多个子项目(如1个应用程序+多个共享库)组成的大项目
TEMPLATE    =   subdirs

#大项目包含的各个子项目
SUBDIRS =   InterceptScreen/GdiGrab \
            NetworkQuality/TcpTest \
            VideoDisplay/PicSendUI \
#            VideoDisplay/SdlPlay \
            VideoEncode/AvEncode \
            PushFlow/RtmpPush


#CONFIG选项要求各个子项目按顺序编译，子目录的编译顺序在SUBDIRS中指明
CONFIG  +=  ordered

# Input
