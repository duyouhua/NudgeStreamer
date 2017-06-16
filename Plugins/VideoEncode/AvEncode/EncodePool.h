/*
 * 单个编码类以及编码池类，在视频编码模块
 * 初始化时就会初始化编码池，同时编码池就
 * 会初始化4个不同码率的编码对象，当网络
 * 质量参数改变时则会在编码池中获取相应码
 * 率的编码对象
 */

#ifndef _ENCODE_POOL_H
#define _ENCODE_POOL_H

#include <memory>
#include <vector>

struct AVCodec;
struct AVFrame;
struct AVPacket;
struct AVCodecContext;

namespace Encode
{
    const int DEFAULT_ENCODER_NUMBER = 4;

    class VideoEncoder
    {
    public:
        VideoEncoder();
        ~VideoEncoder();

    public:
        bool Initial(const int& width, const int& height, const int& i);
        void StartEncode(const std::string& class_name);

    private:
        void SetEncodeParam(const int& i);
        void SaveEncodeData();

    private:
        bool isInit;
        AVCodec* _codec;// 编码器
        AVPacket* _packet;
        AVFrame* _frame_yuv;
        AVCodecContext* _codec_context;
        std::string class_name;
#ifdef OUT_FILE_H264
        FILE* _file_out;
#endif
    };

    typedef std::shared_ptr<VideoEncoder> AvEncoderPtr;

    class EncodePool
    {
    public:
        bool Initial(const int& width, const int& height);

        static EncodePool& GetInstance()
        {
            static EncodePool inc;
            return inc;
        }

        AvEncoderPtr GetCoder(const int& encoder_id)
        {
            return vec_encoders[encoder_id];
        }

    private:
        EncodePool(const EncodePool&) = delete;
        EncodePool& operator=(const EncodePool&) = delete;

        EncodePool() : class_name("EncodePool") {}
        ~EncodePool() {}

    private:
        std::string class_name;
        std::vector<AvEncoderPtr> vec_encoders;
    };
}

#endif // _ENCODE_POOL_H
