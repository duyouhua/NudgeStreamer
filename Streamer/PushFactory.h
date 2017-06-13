/*
 * 视频推流工厂，界面层持有视频推流抽象类的指针，
 * 通过调用该工厂类获取推流模块实现的类
 */

#ifndef _PUSH_FACTORY_H
#define _PUSH_FACTORY_H

#include "PushFlow.h"
#include <string>

namespace Push
{
    class PushFactory
    {
    public:
        PushFactory();
        ~PushFactory();

    public:
        PushFlow* CreatePushClass(const std::string& type, const std::string& url);
    };
}

#endif // _PUSH_FACTORY_H
