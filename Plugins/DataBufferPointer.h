/*
 * 持有所有消息队列的指针，这个类是个单例类，
 * 各个模块通过获取该对象的地址可以获取相应
 * 消息队列的指针，编写该类主要目的是防止客
 * 户端在关闭时销毁子线程的时候崩溃的情况；
 * 上面所说的崩溃情况主要是指：当关闭客户端
 * 的时候，程序会先销毁子线程，再销毁父线程，
 * 而程序关闭释放资源时，会先释放消息队列类
 * 模板，这时子线程有可能刚好运行到读取消息
 * 队列的内容，而消息队列的类模板早已经释放
 * 资源了，此时获取消息队列的地址是空的，从
 * 而导致程序在关闭的时候会崩溃，为了解决这
 * 个崩溃问题而写出了这个单例类，因为类模板
 * 不能写成单例类，静态变量是在编译期间就会
 * 给定地址空间，而类模板是在运行期间有实际
 * 变量才可以生成对应的对象，这个单例类相当
 * 于将消息队列的资源释放推迟了，等到子线程
 * 释放完毕才会对该单例类进行资源释放，从而
 * 避免了程序崩溃问题
 */

#ifndef _DATA_BUFFER_POINTER_H
#define _DATA_BUFFER_POINTER_H

#include "LogMessage.h"
#include "DataBufferQueue.h"

struct Frame;
struct PicYUV;

class DataBufferPointer
{
public:
    static DataBufferPointer& GetInstance()
    {
        static DataBufferPointer inc;
        return inc;
    }

    DataBufferQueue<PicYUV>* GetPicYUVData()
    {
        return p_yuv_buffer;
    }

    DataBufferQueue<NetworkParament>* GetNetworkParam()
    {
        return p_network_buffer;
    }

    DataBufferQueue<Frame>* GetEncodeData()
    {
        return p_encode_buffer;
    }

    DataBufferQueue<Logger::LogMessage>* GetLog()
    {
        return p_log_buffer;
    }

private:
    DataBufferPointer(const DataBufferPointer&) = delete;
    DataBufferPointer& operator=(const DataBufferPointer&) = delete;

    DataBufferPointer()
    {
        p_yuv_buffer = new DataBufferQueue<PicYUV>;
        p_network_buffer = new DataBufferQueue<NetworkParament>;
        p_encode_buffer = new DataBufferQueue<Frame>;
        p_log_buffer = new DataBufferQueue<Logger::LogMessage>;
    }

    ~DataBufferPointer()
    {
        if (p_network_buffer)
        {
            delete p_network_buffer;
            p_network_buffer = NULL;
        }

        if (p_yuv_buffer)
        {
            delete p_yuv_buffer;
            p_yuv_buffer = NULL;
        }

        if (p_encode_buffer)
        {
            delete p_encode_buffer;
            p_encode_buffer = NULL;
        }

        if(p_log_buffer)
        {
            delete p_log_buffer;
            p_log_buffer = NULL;
        }
    }

private:
    DataBufferQueue<PicYUV>* p_yuv_buffer;
    DataBufferQueue<Frame>* p_encode_buffer;
    DataBufferQueue<Logger::LogMessage>* p_log_buffer;
    DataBufferQueue<NetworkParament>* p_network_buffer;
};

#endif // _DATA_BUFFER_POINTER_H
