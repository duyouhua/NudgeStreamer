/*
 * 自定义网络参数，网络计算模块连接服务器计算出时间结果
 * 后将会存到该结构体，然后存入消息队列中，编码对象将会
 * 定时从消息队列中取出该存储的结构体，读取数据比较并进
 * 行编码码率的调整
 */

#ifndef _NETWORK_PARAMENT_H
#define _NETWORK_PARAMENT_H

struct NetworkParament
{
    int _min;// 网络连接最小时间值
    int _max;// 网络连接最大时间值
    int _delay;// 网络连接平均值
    int _loss;// 丢包率，取值范围为0~100，表示百分之多少的丢包率
    int _minus_jitter;// 网络抖动负值
    int _plus_jitter;// 网络抖动正值

    NetworkParament()
    {
        _min = _max = _delay = 0;
        _minus_jitter = _plus_jitter = 0;
    }

    NetworkParament(const int& min, const int& max, const int& delay, const int& loss) :
        _min(min),
        _max(max),
        _delay(delay),
        _loss(loss)
    {
        _minus_jitter = _min - _delay;
        _plus_jitter = _max - _delay;
    }

    // 计算网络抖动区间
    int JitterRange()
    {
        return _plus_jitter - _minus_jitter;
    }
};

#endif // _NETWORK_PARAMENT_H
