/*
 * 视频编码工厂，界面层持有编码抽象类的指针，
 * 通过调用该工厂类获取编码模块实现的类
 */

#ifndef _ENCODE_FACTORY_H
#define _ENCODE_FACTORY_H

#include "VideoEncode.h"
#include <string>

namespace Encode
{
    class EncodeFactory
    {
    public:
        EncodeFactory();
        ~EncodeFactory();

    public:
        // 使用该方法获取编码模块实现方式的对象
        VideoEncode* CreateEncodeClass(const std::string& type, const int& width, const int& height);
    };
}

#endif // _ENCODE_FACTORY_H
