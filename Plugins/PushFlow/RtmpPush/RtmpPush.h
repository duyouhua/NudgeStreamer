#ifndef _RTMP_PUSH_H
#define _RTMP_PUSH_H

#include "PushFlow.h"
#include <mutex>

struct RTMP;

namespace Push
{
    class RtmpPush : public PushFlow
    {
    public:
        RtmpPush(const std::string& url);
        ~RtmpPush();

    public:
        bool Initial();

    protected:
        void run();

    private:
        bool RTMPConnect();// 初始化并连接到服务器
        bool RTMPSend(uint8_t* data, const uint32_t& size, const bool& keyFrame, const uint32_t& timeStamp);// 将内存中的一段H.264编码的视频数据利用RTMP协议发送到服务器
        void RTMPClose();// 断开连接，释放相关的资源

        int GetFrameType(const uint8_t* data);
        bool SendPacket(uint8_t* data, const uint32_t& size, const uint32_t& timeStamp, const uint32_t& packetType);// 发送RTMP数据包
        bool SendVideoSpsPps(uint8_t* pps, const int& pps_len, uint8_t* sps, const int& sps_len);// 发送视频的sps和pps信息

    private:
        struct RTMPMetaData
        {
            int width;
            int height;
            int frame_rate;// fps
            int data_rate;// bps
            uint32_t sps_len;
            uint32_t pps_len;
            uint8_t* sps;
            uint8_t* pps;

            RTMPMetaData() :
                width(0),
                height(0),
                frame_rate(0),
                data_rate(0),
                sps_len(0),
                pps_len(0),
                sps(NULL),
                pps(NULL)
            {
            }

            ~RTMPMetaData()
            {
                width = 0;
                height = 0;
                frame_rate = 0;
                data_rate = 0;
                sps_len = 0;
                pps_len = 0;

                if (sps)
                {
                    delete[] sps;
                    sps = NULL;
                }

                if (pps)
                {
                    delete[] pps;
                    pps = NULL;
                }
            }
        };

        struct NaluUnit
        {
            int type;
            int size;
            uint8_t* data;

            NaluUnit() : type(0), size(0), data(NULL) {}

            NaluUnit(const int& size) : type(0), size(size)
            {
                data = new uint8_t[size];
            }

            ~NaluUnit()
            {
                if (data)
                {
                    delete[] data;
                    data = NULL;
                }

                type = 0;
                size = 0;
            }
        };

    private:
        // 定义包头长度，RTMP_MAX_HEADER_SIZE = 18
        const int RTMP_HEAD_SIZE;

    private:
        uint32_t m_buffer_size;
        RTMP* p_rtmp;
        uint8_t* p_buffer;
        char* p_url_addr;
        RTMPMetaData* p_meta_data;
        std::mutex m_mutex;
    };
}

#endif // _RTMP_PUSH_H
