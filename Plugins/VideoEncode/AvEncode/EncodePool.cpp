#include "Frame.h"
#include "PicYUV.h"
#include "Logger.h"
#include "EncodePool.h"
#include "DataBufferPointer.h"

#ifdef OUT_FILE_H264
#include <iostream>
#include <QString>
#endif

#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

namespace Encode
{
    struct EncodeParam
    {
        // 每gop_size帧插入1个关键帧，关键帧越少，视频越小
        int gop_size;
        // 固定允许的码率误差，数值越大，视频越小
        int bit_rate_tolerance;
        // 最小的量化因子，该数字越小质量越好，取值范围为0-51，0为无损，23是默认值，18-28合理范围
        int qmin;
        int qmax;
        // 浮点数值范围在0.0-1.0之间，表示在压制“容易压缩的场景”和“难压缩的场景”时，允许Q值的变化范围
        float qcompress;
        // 因为我们的量化系数q是在qmin和qmax之间浮动的，qblur表示这种浮动变化的变化程度，取值范围0.0～1.0，取0表示不削减
        float qblur;
        // 目标码率，即采样码率；采样码率越大，视频容量越大
        int64_t bit_rate;
    };

    // 编码参数，表示不同码率，该数组下标越小，码率越大
    const EncodeParam _param[DEFAULT_ENCODER_NUMBER] =
        {
            { 15, 5000000, 10, 20, (float)0.2, (float)0.2, 5000000 },
            { 20, 4000000, 20, 30, (float)0.4, (float)0.4, 4000000 },
            { 25, 3000000, 30, 40, (float)0.6, (float)0.6, 3000000 },
            { 30, 2000000, 40, 45, (float)0.8, (float)0.8, 2000000 }
        };

    VideoEncoder::VideoEncoder() :
        b_isInit(false),
        m_class_name("VideoEncoder")
    {
        p_codec_context = NULL;
        p_codec = NULL;
        p_packet = NULL;
        p_frame_yuv = NULL;
    }

    VideoEncoder::~VideoEncoder()
    {
        av_free(p_frame_yuv);
        avcodec_close(p_codec_context);
        av_free(p_codec_context);
#ifdef OUT_FILE_H264
        fclose(file_out);
#endif
    }

    bool VideoEncoder::Initial(const int& width, const int& height, const int& i)
    {
        // 检查是否已经进行了初始化，已经初始化了就返回
        if (b_isInit)
            return true;

        av_register_all();
        avcodec_register_all();
        p_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!p_codec)
        {
            // 异常处理
            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    m_class_name,
                    "H264 codec not found",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        p_codec_context = avcodec_alloc_context3(p_codec);
        p_codec_context->codec_type = AVMEDIA_TYPE_VIDEO;
        p_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
        p_codec_context->width = width;
        p_codec_context->height = height;
        // 两个非B帧之间允许出现多少个B帧数，设置0表示不使用B帧，
        // 没有延迟，B帧越多，容量越小，及压缩效果最好，但延迟也较多
        p_codec_context->max_b_frames = 1;
        // 帧率的基本单位，表示1秒25帧
        p_codec_context->time_base.num = 1;
        p_codec_context->time_base.den = 25;
        // 视频中所有祯（包括i/b/p）的最大Q值差距，
        // 在帧与帧之间进行切变的量化因子的最大变化量
        p_codec_context->max_qdiff = 4;
        p_codec_context->me_range = 16;
        SetEncodeParam(i);

        AVDictionary* _parament = 0;
        if (AV_CODEC_ID_H264 == p_codec_context->codec_id)
        {
            av_dict_set(&_parament, "preset", "fast", 0);
            av_dict_set(&_parament, "tune", "zerolatency", 0);
            av_dict_set(&_parament, "profile", "main", 0);
        }

        if (avcodec_open2(p_codec_context, p_codec, &_parament) < 0)
        {
            // 异常处理
            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    m_class_name,
                    "can not open encoder",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        p_frame_yuv = av_frame_alloc();
        p_packet = (AVPacket*)av_malloc(sizeof(AVPacket));

        av_dict_free(&_parament);

        b_isInit = true;

#ifdef OUT_FILE_H264
        QString path = QString("D:/OUT_H264_") + QString::number(i) + QString(".h264");
        file_out = fopen(path.toStdString().c_str(), "wb+");
#endif
        Logger::LogMessage* log
            = new Logger::LogMessage
                (
                Logger::INFO,
                m_class_name,
                "initial successfully",
                QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                );
        DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

        return true;
    }

    void VideoEncoder::StartEncode(const std::string& class_name)
    {
        if (DataBufferPointer::GetInstance().GetPicYUVData()->HaveData(class_name))
        {
            std::shared_ptr<PicYUV> yuv_data = DataBufferPointer::GetInstance().GetPicYUVData()->PopTop(class_name);
            if (yuv_data->data)
            {
                p_frame_yuv->width = yuv_data->width;
                p_frame_yuv->height = yuv_data->height;
                p_frame_yuv->format = AV_PIX_FMT_YUV420P;
                int _malloc_size = avpicture_get_size(p_codec_context->pix_fmt, p_codec_context->width, p_codec_context->height);
                uint8_t* _buff = (uint8_t*)av_malloc(_malloc_size);
                avpicture_fill((AVPicture*)p_frame_yuv, _buff, p_codec_context->pix_fmt, p_codec_context->width, p_codec_context->height);

                if (av_new_packet(p_packet, _malloc_size))
                {
                    // 异常处理
                }

                int _size = yuv_data->width * yuv_data->height;
                memcpy(p_frame_yuv->data[0], yuv_data->data[0], _size);
                memcpy(p_frame_yuv->data[1], yuv_data->data[1], _size / 4);
                memcpy(p_frame_yuv->data[2], yuv_data->data[2], _size / 4);

                int got_picture = 0;
                // Encode
                int ret = avcodec_encode_video2(p_codec_context, p_packet, p_frame_yuv, &got_picture);
                if (ret < 0)
                {
                    // Error
                    return;
                }

                if (got_picture)
                {
                    SaveEncodeData();
#ifdef OUT_FILE_H264
                    fwrite(p_packet->data, 1, p_packet->size, file_out);
#endif
                }

                av_free(_buff);
                av_free_packet(p_packet);
            }
        }
    }

    inline void VideoEncoder::SaveEncodeData()
    {
        Frame* frame = new Frame(p_packet->size);
        memcpy(frame->data, p_packet->data, p_packet->size);
        DataBufferPointer::GetInstance().GetEncodeData()->PushBack(std::shared_ptr<Frame>(frame));
    }

    void VideoEncoder::SetEncodeParam(const int& i)
    {
        p_codec_context->bit_rate = _param[i].bit_rate;
        p_codec_context->bit_rate_tolerance = _param[i].bit_rate_tolerance;
        p_codec_context->gop_size = _param[i].gop_size;
        p_codec_context->qcompress = _param[i].qcompress;
        p_codec_context->qmin = _param[i].qmin;
        p_codec_context->qmax = _param[i].qmax;
        p_codec_context->qblur = _param[i].qblur;
    }

    bool EncodePool::Initial(const int& width, const int& height)
    {
        for (int i = 0; i < DEFAULT_ENCODER_NUMBER; ++i)
        {
            VideoEncoder* _encoder = new VideoEncoder;
            if (_encoder->Initial(width, height, i))
            {
                m_vec_encoders.push_back(AvEncoderPtr(_encoder));
            }
            else
            {
                Logger::LogMessage* log
                    = new Logger::LogMessage
                        (
                        Logger::WARNING,
                        m_class_name,
                        "initial failure",
                        QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                        );
                DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

                return false;
            }
        }

        Logger::LogMessage* log
            = new Logger::LogMessage
                (
                Logger::INFO,
                m_class_name,
                "initial successfully",
                QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                );
        DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

        return true;
    }
}
