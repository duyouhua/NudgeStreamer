#include "Logger.h"
#include "AvEncode.h"
#include "EncodePool.h"

#include <QTime>

namespace Encode
{
    AvEncode::AvEncode(const int& width, const int& height) :
        DEFAULT_TIME(2000),
        DEFAULT_WIDTH(width),
        DEFAULT_HEIGHT(height),
        m_current_encoder_id(1)
    {
        setObjectName("AvEncode");
    }

    AvEncode::~AvEncode()
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

    bool AvEncode::Initial()
    {
        if (EncodePool::GetInstance().Initial(DEFAULT_WIDTH, DEFAULT_HEIGHT))
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
        else
        {
            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::WARNING,
                    objectName().toStdString(),
                    "initial failure",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }
    }

    void AvEncode::run()
    {
        m_current_encoder_id = GetEncoderID();
        AvEncoderPtr _encoder = EncodePool::GetInstance().GetCoder(m_current_encoder_id);

        QTime _time_count;
        int _time_sum = 0;
        _time_count.start();

        while (m_shut_down)
        {
            _time_sum += _time_count.elapsed();
            _time_count.restart();
            if (_time_sum >= DEFAULT_TIME)
            {
                _time_sum = 0;
                int id = GetEncoderID();
                if (m_current_encoder_id != id)
                {
                    m_current_encoder_id = id;
                    _encoder = EncodePool::GetInstance().GetCoder(m_current_encoder_id);
                }
            }
            _encoder->StartEncode(ClassName());
        }
    }

    // 获取编码ID
    int AvEncode::GetEncoderID()
    {
        // 根据网络质量参数判断当前网络如何，从而获取编码码率对象的ID，通过ID可以在编码池中拿到对应码率的编码对象
        if (DataBufferPointer::GetInstance().GetNetworkParam()->HaveData(ClassName()))
        {
            std::shared_ptr<NetworkParament> _net_elem = DataBufferPointer::GetInstance().GetNetworkParam()->PopTop(ClassName());
            if (_net_elem->_delay >= 0 && _net_elem->_delay <= 30)// 极快，几乎察觉不出有延迟，顺畅
            {
                return 0;
            }
            else if (_net_elem->_delay >= 31 && _net_elem->_delay <= 50)// 良好，正常没有明显的延迟情况
            {
                return 1;
            }
            else if (_net_elem->_delay >= 51 && _net_elem->_delay <= 100)// 普通，稍有停顿
            {
                return 2;
            }
            else// 差，有卡顿，丢包并掉线现象
            {
                return 3;
            }
        }
        return 1;
    }
}
