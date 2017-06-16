/*
 * 计算网络质量的抽象类，提供了计算网络质量以及初始化的接口；
 * 实现该模块功能必须继承该抽象类
*/

#ifndef _NETWORK_QUALITY_H
#define _NETWORK_QUALITY_H

#include "DataToUI.h"
#include <QThread>

struct NetworkParament;

class NetworkQuality : public QThread
{
public:
    NetworkQuality() : shut_down(true) {}

public:
    virtual bool Initial() = 0;

    void ShutDown()
    {
        shut_down = false;
    }

protected:
    void ThrowError(const QString& err)
    {
        DataToUI::GetInstance().setParament(err);
    }

    void SendMessage(const NetworkParament& parament)
    {
        DataToUI::GetInstance().setParament(parament);
    }

protected:
    bool shut_down;
};

#endif // _NETWORK_QUALITY_H
