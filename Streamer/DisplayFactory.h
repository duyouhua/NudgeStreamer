/*
 * 视频显示工厂，界面层持有显示抽象类的指针，
 * 通过调用该工厂类获取模块实现的类
 */

#ifndef _DISPLAY_FACTORY_H
#define _DISPLAY_FACTORY_H

#include "VideoDisplay.h"
#include <string>

namespace Display
{
    class DisplayFactory
    {
    public:
        DisplayFactory();
        ~DisplayFactory();

    public:
        VideoDisplay* CreateDisplayClass(const std::string& type, const int& width, const int& height, QWidget* widget = NULL);
    };
}

#endif // _DISPLAY_FACTORY_H
