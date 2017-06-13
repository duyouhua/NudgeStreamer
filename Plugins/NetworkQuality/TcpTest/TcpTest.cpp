#include "Logger.h"
#include "TcpTest.h"
#include "NetworkParament.h"
#include "DataBufferPointer.h"

#include <QTime>
#include <QTcpSocket>

namespace Network
{
    TcpTest::TcpTest(const std::string& ip, const unsigned short& port) :
         DEFAULT_HOST(ip.c_str()),
         DEFAULT_PORT(port),
         DEFAULT_CONNECT_COUNT(40),
         DEFAULT_WAIT_TIMES(500)
    {
        setObjectName("TcpTest");
    }

    TcpTest::~TcpTest()
    {
        Logger::LogMessage* log
            = new Logger::LogMessage
                (
                Logger::INFO,
                objectName().toStdString(),
                "is closing",
                QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                );
        DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));
    }

    bool TcpTest::Initial()
    {
        Logger::LogMessage* log
            = new Logger::LogMessage
                (
                Logger::INFO,
                objectName().toStdString(),
                "initial successfully",
                QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                );
        DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

        return true;
    }

    void TcpTest::run()
    {
        QTcpSocket tcp_connect;
        while (m_shut_down)
        {
            int success_count = 0, unsuccess_count = 0;
            int min = 999999999, max = 0, delay = 0;
            QTime time_count;
            /*
             * 运用Tcp连接服务器，连接后会得出一个时间间隔，断开与服务器的连接
             * 进行4次连接，比较得出最小值、最大值，将时间间隔加起来可算
             * 出平均值
             */
            for (int i = 0; i < DEFAULT_CONNECT_COUNT; ++i)
            {
                tcp_connect.abort();
                if (0 == i)
                    time_count.start();
                else
                    time_count.restart();
                tcp_connect.connectToHost(DEFAULT_HOST, DEFAULT_PORT);
                if (tcp_connect.waitForConnected(DEFAULT_WAIT_TIMES))
                {
                    int success_time = time_count.elapsed();
                    delay += success_time;
                    min = success_time < min ? success_time : min;
                    max = success_time > max ? success_time : max;
                    ++success_count;

                    tcp_connect.disconnectFromHost();
                }
                else
                {
                    time_count.elapsed();
                    ++unsuccess_count;
                }
            }

            try
            {
                if (!success_count)
                    throw success_count;
                delay /= success_count;
            }
            catch (int&)
            {
                // 成功连接次数为0，说明四次tcp连接都
                // 不成功，将连接抛异常给界面层处理
                ThrowError(QString("Couldn't connect to server, please check your connection."));

                Logger::LogMessage* log
                    = new Logger::LogMessage
                        (
                        Logger::ERRORINFOR,
                        objectName().toStdString(),
                        "couldn't conect to server",
                        QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                        );
                DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

                return;
            }

            // 发送数据给主线程进行显示
            NetworkParament* parament = new NetworkParament(min, max, delay, 100 * unsuccess_count / DEFAULT_CONNECT_COUNT);
            SendMessage(*parament);

            /*
             * 将min、max、avg三个值依次存入消息队列中，
             * 编码的类会在一段时间内读取该消息队列，从
             * 中获取数据后比较延时和抖动值从而改变编码
             * 码率
             */
            DataBufferPointer::GetInstance().GetNetworkParam()->PushBack(std::shared_ptr<NetworkParament>(parament));
        }
        tcp_connect.close();
    }
}
