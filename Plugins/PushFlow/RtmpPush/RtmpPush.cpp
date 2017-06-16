#include "Frame.h"
#include "Logger.h"
#include "RtmpPush.h"
#include "SpsDecode.h"

extern "C"
{
#include <amf.h>
#include <log.h>
#include <rtmp.h>
#include <rtmp_sys.h>
}

namespace Push
{
    RtmpPush::RtmpPush(const std::string& url) :
        RTMP_HEAD_SIZE(sizeof(RTMPPacket) + RTMP_MAX_HEADER_SIZE)
    {
        setObjectName("RtmpPush");

        _url_addr = new char[url.size() + 1];
        strcpy(_url_addr, url.c_str());

        _buffer = NULL;
        _meta_data = NULL;
    }

    RtmpPush::~RtmpPush()
    {
        if (_url_addr)
        {
            delete _url_addr;
            _url_addr = NULL;
        }

        if (_buffer)
        {
            delete[] _buffer;
            _buffer = NULL;
        }

        if (_meta_data)
        {
            delete _meta_data;
            _meta_data = NULL;
        }

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

    bool RtmpPush::Initial()
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

    void RtmpPush::run()
    {
#ifdef FILE_H264_4_TEST
        FILE* out_file = fopen("E:/OUT_FILE.h264", "wb+");
#endif
        if (!RTMPConnect())
        {
            // 异常处理
            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    objectName().toStdString(),
                    "can not connect to host",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return;
        }

        uint32_t now, last_update;
        uint32_t tick = 0, tick_gap = 0;
        bool key_frame = false;
        int i = 0;

        // 发送数据
        while (_is_push)
        {
            if (DataBufferPointer::GetInstance().GetEncodeData()->HaveData(ClassName()))
            {
                tick += tick_gap;
                now = RTMP_GetTime();

                std::shared_ptr<Frame> frame = DataBufferPointer::GetInstance().GetEncodeData()->PopTop(ClassName());
                std::lock_guard<std::mutex> lock(_mutex);
                _buffer = new uint8_t[frame->size];
                memcpy(_buffer, frame->data, frame->size);
                buffer_size = frame->size;

                NaluUnit* nalu_unit;
                // 查看是否含有sps、pps帧，有就截取出来单独发送到服务器
                if (0x67 == _buffer[GetFrameType(_buffer)])
                {
                    uint32_t i = 0, begin = 0, end = 0;
                    _meta_data = new RTMPMetaData;
                    while ((_meta_data && i < buffer_size - 4) || 0x06 == _buffer[GetFrameType(_buffer)])
                    {
                        int ret = GetFrameType(_buffer + i);
                        if (ret != -1 || buffer_size - 4 == i)
                        {
                            if (0 == begin && 0 == end && 0 == i)
                            {
                                i += ret;
                                continue;
                            }

                            end = i - 1;
                            int size = end + 1 - begin;
                            nalu_unit = new NaluUnit(size);
                            memcpy(nalu_unit->data, _buffer + begin, size);
                            nalu_unit->type = nalu_unit->data[GetFrameType(nalu_unit->data)] & 0x1f;
                            begin = i;

                            if (0x07 == nalu_unit->type)
                            {
                                _meta_data->sps = new uint8_t[nalu_unit->size];
                                memcpy(_meta_data->sps, nalu_unit->data, nalu_unit->size);
                                _meta_data->sps_len = nalu_unit->size;

                                // 解码SPS,获取视频图像宽、高信息
                                h264_decode_sps(_meta_data->sps, _meta_data->sps_len, _meta_data->width, _meta_data->height, _meta_data->frame_rate);

                                if (!_meta_data->frame_rate)
                                    _meta_data->frame_rate = 25;

                                tick = 0;
                                tick_gap = 1000 / _meta_data->frame_rate;
                            }
                            else if (0x08 == nalu_unit->type)
                            {
                                _meta_data->pps = new uint8_t[nalu_unit->size];
                                memcpy(_meta_data->pps, nalu_unit->data, nalu_unit->size);
                                _meta_data->pps_len = nalu_unit->size;

                                buffer_size -= (_meta_data->sps_len + _meta_data->pps_len);
                                uint8_t* temp = new uint8_t[buffer_size];
                                memcpy(temp, _buffer + i, buffer_size);
                                delete[] _buffer;
                                _buffer = temp;

                                i = 0;
                                begin = end = 0;

#ifdef FILE_H264_4_TEST
                                fwrite(_meta_data->sps, 1, _meta_data->sps_len, out_file);
                                fwrite(_meta_data->pps, 1, _meta_data->pps_len, out_file);
#endif

                                if (SendVideoSpsPps(_meta_data->pps, _meta_data->pps_len, _meta_data->sps, _meta_data->sps_len))
                                {
                                    delete _meta_data;
                                    _meta_data = NULL;
                                }
                            }
                            else if (0x06 == nalu_unit->type)
                            {
                                key_frame = false;
                                RTMPSend(nalu_unit->data, nalu_unit->size, key_frame, tick);
#ifdef FILE_H264_4_TEST
                                fwrite(nalu_unit->data, 1, nalu_unit->size, out_file);
#endif
                                buffer_size -= nalu_unit->size;
                                uint8_t* temp = new uint8_t[buffer_size];
                                memcpy(temp, _buffer + begin, buffer_size);
                                delete[] _buffer;
                                _buffer = temp;
                            }

                            if (nalu_unit)
                            {
                                delete nalu_unit;
                                nalu_unit = NULL;
                            }
                        }

                        if (-1 == ret)
                        {
                            ++i;
                        }
                        else
                        {
                            i += ret;
                        }
                    }
                }

                nalu_unit = new NaluUnit(buffer_size);
                memcpy(nalu_unit->data, _buffer, buffer_size);
                nalu_unit->type = nalu_unit->data[GetFrameType(nalu_unit->data)] & 0x1f;

                key_frame = (0x05 == nalu_unit->type) ? true : false;
#ifdef FILE_H264_4_TEST
                fwrite(nalu_unit->data, 1, nalu_unit->size, out_file);
#endif
                RTMPSend(nalu_unit->data, nalu_unit->size, key_frame, tick);
                last_update = RTMP_GetTime();

                if (nalu_unit)
                {
                    delete nalu_unit;
                    nalu_unit = NULL;
                }

                if (_buffer)
                {
                    delete[] _buffer;
                    _buffer = NULL;

                    buffer_size = 0;
                }

                msleep(tick_gap  - now + last_update);
            }
        }

        RTMPClose();
#ifdef FILE_H264_4_TEST
        fclose(out_file);
#endif
    }

    bool RtmpPush::RTMPConnect()
    {
        _rtmp = RTMP_Alloc();
        RTMP_Init(_rtmp);

        // 设置URL
        if (!RTMP_SetupURL(_rtmp, _url_addr))
        {
            RTMP_Free(_rtmp);

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    "RTMP",
                    "fail to set up url",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        // 设置可写,即发布流,这个函数必须在连接前使用,否则无效
        RTMP_EnableWrite(_rtmp);
        // 连接服务器
        if (!RTMP_Connect(_rtmp, NULL))
        {
            RTMP_Free(_rtmp);

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    "RTMP",
                    "fail to connect to host",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        // 连接流
        if (!RTMP_ConnectStream(_rtmp, 0))
        {
            RTMP_Close(_rtmp);
            RTMP_Free(_rtmp);

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    "RTMP",
                    "fail to create connect stream",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        return true;
    }

    bool RtmpPush::RTMPSend(uint8_t* data, const uint32_t& size, const bool& keyFrame, const uint32_t& timeStamp)
    {
        if (!data && size < 11)
        {
            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::WARNING,
                    "RTMPPacket data",
                    "is too little",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        uint8_t* _body = new uint8_t[size + 9];
        memset(_body, 0, size + 9);

        int i = 0;
        uint32_t cp_size = size;

        if (keyFrame)
        {
            _body[i++] = 0x17;// 1:Iframe  7:AVC
            _body[i++] = 0x01;// AVC NALU
            _body[i++] = 0x00;
            _body[i++] = 0x00;
            _body[i++] = 0x00;

            // NALU size
            _body[i++] = cp_size >> 24 & 0xff;
            _body[i++] = cp_size >> 16 & 0xff;
            _body[i++] = cp_size >> 8 & 0xff;
            _body[i++] = cp_size & 0xff;
            // NALU data
            memcpy(&_body[i], data, cp_size);
        }
        else
        {
            _body[i++] = 0x27;// 2:Pframe  7:AVC
            _body[i++] = 0x01;// AVC NALU
            _body[i++] = 0x00;
            _body[i++] = 0x00;
            _body[i++] = 0x00;

            // NALU size
            _body[i++] = cp_size >> 24 & 0xff;
            _body[i++] = cp_size >> 16 & 0xff;
            _body[i++] = cp_size >> 8 & 0xff;
            _body[i++] = cp_size & 0xff;
            // NALU data
            memcpy(&_body[i], data, cp_size);
        }

        bool result = SendPacket(_body, i + cp_size, timeStamp, RTMP_PACKET_TYPE_VIDEO);
        delete[] _body;

        return result;
    }

    void RtmpPush::RTMPClose()
    {
        if (_rtmp)
        {
            RTMP_Close(_rtmp);
            RTMP_Free(_rtmp);
            _rtmp = NULL;

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::INFO,
                    "RTMP",
                    "is closing",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));
        }
    }

    int RtmpPush::GetFrameType(const uint8_t* data)
    {
        if (0x00 == data[0] && 0x00 == data[1] && 0x01 == data[2])
        {
            return 3;
        }
        else if (0x00 == data[0] && 0x00 == data[1] && 0x00 == data[2] && 0x01 == data[3])
        {
            return 4;
        }
        else
        {
            return -1;
        }
    }

    /*
     * 发送视频的sps和pps信息
     *
     * @param pps 存储视频的pps信息
     * @param pps_len 视频的pps信息长度
     * @param sps 存储视频的pps信息
     * @param sps_len 视频的sps信息长度
     *
     */
    bool RtmpPush::SendVideoSpsPps(uint8_t* pps, const int& pps_len, uint8_t* sps, const int& sps_len)
    {
        RTMPPacket* _packet = new RTMPPacket[RTMP_HEAD_SIZE + 1024];
        memset(_packet, 0, RTMP_HEAD_SIZE + 1024);
        _packet->m_body = (char*)_packet + RTMP_HEAD_SIZE;

        uint8_t* _body = (uint8_t*)_packet->m_body;

        int i = 0;
        _body[i++] = 0x17;
        _body[i++] = 0x00;

        _body[i++] = 0x00;
        _body[i++] = 0x00;
        _body[i++] = 0x00;

        // AVCDecoderConfigurationRecord
        _body[i++] = 0x01;
        _body[i++] = sps[1];
        _body[i++] = sps[2];
        _body[i++] = sps[3];
        _body[i++] = 0xff;

        /*sps*/
        _body[i++]   = 0xe1;
        _body[i++] = (sps_len >> 8) & 0xff;
        _body[i++] = sps_len & 0xff;
        memcpy(&_body[i], sps, sps_len);
        i +=  sps_len;

        /*pps*/
        _body[i++]   = 0x01;
        _body[i++] = (pps_len >> 8) & 0xff;
        _body[i++] = (pps_len) & 0xff;
        memcpy(&_body[i], pps, pps_len);
        i +=  pps_len;

        _packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
        _packet->m_nBodySize = i;
        _packet->m_nChannel = 0x04;
        _packet->m_nTimeStamp = 0;
        _packet->m_hasAbsTimestamp = 0;
        _packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
        _packet->m_nInfoField2 = _rtmp->m_stream_id;

        // 调用发送接口
        int result = RTMP_SendPacket(_rtmp, _packet, TRUE);
        delete[] _packet;   //释放内存

        return result;
    }

    /*
     * 发送RTMP数据包
     *
     * @param packetType 数据类型
     * @param data 存储数据内容
     * @param size 数据大小
     * @param timeStamp 当前包的时间戳
     *
     */
    bool RtmpPush::SendPacket(uint8_t* data, const uint32_t& size, const uint32_t& timeStamp, const uint32_t& packetType)
    {
        RTMPPacket* _packet = new RTMPPacket[RTMP_HEAD_SIZE + size];
        memset(_packet, 0, RTMP_HEAD_SIZE);

        _packet->m_body = (char*)_packet + RTMP_HEAD_SIZE;
        _packet->m_nBodySize = size;
        memcpy(_packet->m_body, data, size);
        _packet->m_hasAbsTimestamp = 0;
        _packet->m_packetType = packetType;
        _packet->m_nInfoField2 = _rtmp->m_stream_id;
        _packet->m_nChannel = 0x04;

        _packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
        _packet->m_nTimeStamp = timeStamp;

        int result = 0;
        if (RTMP_IsConnected(_rtmp))
        {
            result = RTMP_SendPacket(_rtmp, _packet, TRUE);// TRUE为放进发送队列,FALSE是不放进发送队列,直接发送
        }
        delete[] _packet;

        return result;
    }
}
