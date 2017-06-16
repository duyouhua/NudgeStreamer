/*
 * 网络质量计算工厂，界面层持有网络质量计算抽象类的指针，
 * 通过调用该工厂类获取网络质量计算模块实现的类
 */

#ifndef _NETWORK_FACTORY_H
#define _NETWORK_FACTORY_H

#include "NetworkQuality.h"
#include <string>

namespace Network
{
    class NetworkFactory
    {
    public:
        NetworkFactory();
        ~NetworkFactory();

    public:
        // 使用该方法获取网络质量计算模块实现方式的对象
        NetworkQuality* CreateNetworkClass(const std::string& type, const std::string& ip, const unsigned short& port);
    };
}

#endif // NETWORK_FACTORY_H
