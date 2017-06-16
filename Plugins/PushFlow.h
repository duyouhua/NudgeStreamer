/*
 * 推流的抽象类，提供了相应接口，成员变量_class_name主要用于对象
 * 获取消息队列中的数据时作为函数入口，消息队列依靠此类名在映射表
 * 上取出在对应队列中的数据，实现类必须继承该抽象类
 */

#ifndef _PUSH_FLOW_H
#define _PUSH_FLOW_H

#include "DataToUI.h"
#include "DataBufferPointer.h"
#include <QThread>

class PushFlow : public QThread
{
public:
    PushFlow() :
        class_name("PushFlow"),
        _is_push(false)
    {
        DataBufferPointer::GetInstance().GetEncodeData()->RegistCustomer(ClassName());
    }

public:
    virtual bool Initial() = 0;

    void StartPush()
    {
        _is_push = true;
    }

    void StopPush()
    {
        _is_push = false;
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
    bool _is_push;

private:
    const std::string class_name;
};


#endif // _PUSH_FLOW_H
