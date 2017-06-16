/*
 * 视频编码类，提供了相应的接口，成员变量类名主要
 * 用于消息队列中获取数据，实现该模块的实现类必须
 * 继承该抽象类
 */

#ifndef _VIDEO_ENCODE_H
#define _VIDEO_ENCODE_H

#include "DataToUI.h"
#include "DataBufferPointer.h"
#include <QThread>

struct PicYUV;
struct NetworkParament;

class VideoEncode : public QThread
{
public:
    VideoEncode() :
        class_name("VideoEncode"),
        shut_down(true)
    {
        DataBufferPointer::GetInstance().GetPicYUVData()->RegistCustomer(ClassName());
        DataBufferPointer::GetInstance().GetNetworkParam()->RegistCustomer(ClassName());
    }

public:
    virtual bool Initial() = 0;

    void ShutDown()
    {
        shut_down = false;
    }

protected:
    std::string ClassName()
    {
        return class_name;
    }

    void ThrowError(const QString& err)
    {
        DataToUI::GetInstance().setParament(err);
    }

protected:
    bool shut_down;

private:
    const std::string class_name;
};

#endif // _VIDEO_ENCODE_H
