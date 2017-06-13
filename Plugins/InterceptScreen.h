/*
 * 截屏的抽象类，提供了对应接口，截屏模块的
 * 实现都需要继承该抽象类
 */

#ifndef _INTERCEPT_SCREEN_H
#define _INTERCEPT_SCREEN_H

#include "DataToUI.h"
#include <QThread>

class InterceptScreen : public QThread
{
public:
    InterceptScreen() : m_shut_down(true) {}

public:
    virtual bool Initial() = 0;

    void ShutDown()
    {
        m_shut_down = false;
    }

protected:
    void ThrowError(const QString& err)
    {
        DataToUI::GetInstance().setParament(err);
    }

protected:
    bool m_shut_down;
};

#endif // _INTERCEPT_SCREEN_H
