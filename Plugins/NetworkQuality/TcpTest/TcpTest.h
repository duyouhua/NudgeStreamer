/*
 * 计算Tcp与服务器三次握手的连接时间，
 * 每次都会连接四次，得出连接最短时长、
 * 最长时长、连接成功次数以及连接失败
 * 次数，并计算网络延时的平均值，可得
 * 出网络延时抖动
 */

#ifndef _TCP_TEST_H
#define _TCP_TEST_H

#include "NetworkQuality.h"

namespace Network
{
    class TcpTest : public NetworkQuality
    {
    public:
        TcpTest(const std::string& ip, const unsigned short& port);
        ~TcpTest();

    public:
        bool Initial();

    protected:
        void run();

    private:
        // 默认的服务器连接次数、IP以及端口
        const int DEFAULT_CONNECT_COUNT;
        const int DEFAULT_WAIT_TIMES;// 连接超时时间
        const QString DEFAULT_HOST;
        const quint16 DEFAULT_PORT;
    };
}

#endif // _TCP_TEST_H
