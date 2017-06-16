/*
 * 截屏工厂，界面层持有截屏抽象类的指针，
 * 通过调用该工厂类获取截屏模块实现的类
 */

#ifndef _INTERCEPT_FACTORY_H
#define _INTERCEPT_FACTORY_H

#include "InterceptScreen.h"
#include <string>

namespace Catpure
{
    class InterceptFactory
    {
    public:
        InterceptFactory();
        ~InterceptFactory();

    public:
        // 使用该方法获取截屏模块实现方式的对象
        InterceptScreen* CreateInterceptClass(const std::string& type, const int& width, const int& height);
    };
}

#endif // _INTERCEPT_FACTORY_H
