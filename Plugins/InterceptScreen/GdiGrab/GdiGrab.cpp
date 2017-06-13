#include "Logger.h"
#include "PicYUV.h"
#include "GdiGrab.h"
#include "DataBufferPointer.h"

#ifdef OUT_FILE_YUV
#include <iostream>
#endif

#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}

namespace Catpure
{
    GdiGrab::GdiGrab(const int& width, const int& height) :
        DEFAULT_WIDTH(width),
        DEFAULT_HEIGHT(height)
    {
        setObjectName("GdiGrab");

        p_format_context = NULL;
        p_codec_context = NULL;
        p_codec = NULL;
        p_frame = NULL;
        p_frame_yuv = NULL;
        img_convert_ctx = NULL;
    }

    GdiGrab::~GdiGrab()
    {
#ifdef OUT_FILE_YUV
        fclose(fp_yuv);
#endif
        sws_freeContext(img_convert_ctx);
        av_free(p_frame_yuv);
        av_free(p_frame);
        avcodec_close(p_codec_context);
        avformat_close_input(&p_format_context);

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

    bool GdiGrab::Initial()
    {
        av_register_all();
        avdevice_register_all();
        avformat_network_init();
        p_format_context = avformat_alloc_context();

        //Use gdigrab
        AVDictionary* options = NULL;
        // 设置帧率为24祯每秒
        av_dict_set(&options, "framerate", "25", 0);
        // 截屏x轴起点
        //av_dict_set(&options, "offset_x", "0", 0);
        // 截屏y轴起点
        //av_dict_set(&options, "offset_y", "0", 0);
        // 截屏大小，默认为全屏
        //av_dict_set(&options, "video_size", "1920X1080", 0);
        AVInputFormat* ifmt = av_find_input_format("gdigrab");
        if (avformat_open_input(&p_format_context, "desktop", ifmt, &options) != 0)
        {
            // 抛异常到界面层，让界面层处理
            ThrowError(QString("Couldn't open input stream."));

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    objectName().toStdString(),
                    "couldn't open input stream",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        if (avformat_find_stream_info(p_format_context, NULL) < 0)
        {
            // 抛异常到界面层，让界面层处理
            ThrowError(QString("Couldn't find stream information."));

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    objectName().toStdString(),
                    "couldn't find stream information",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        for (int i = 0; i < p_format_context->nb_streams; i++)
        {
            if (p_format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                videoindex = i;
                break;
            }
        }

        if (videoindex == -1)
        {
            // 抛异常到界面层，让界面层处理
            ThrowError(QString("Didn't find a video stream."));

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    objectName().toStdString(),
                    "didn't find a video stream",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        p_codec_context = p_format_context->streams[videoindex]->codec;
        p_codec = avcodec_find_decoder(p_codec_context->codec_id);

        if (p_codec == NULL)
        {
            // 抛异常到界面层，让界面层处理
            ThrowError(QString("Codec not found."));

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    objectName().toStdString(),
                    "codec not found",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        if (avcodec_open2(p_codec_context, p_codec, NULL) < 0)
        {
            // 抛异常到界面层，让界面层处理
            ThrowError(QString("Could not open codec."));

            Logger::LogMessage* log
                = new Logger::LogMessage
                    (
                    Logger::ERRORINFOR,
                    objectName().toStdString(),
                    "could not open codec",
                    QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                    );
            DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));

            return false;
        }

        av_dict_free(&options);

        p_frame = av_frame_alloc();
        p_frame_yuv = av_frame_alloc();

        p_packet = (AVPacket*)av_malloc(sizeof(AVPacket));
#ifdef OUT_FILE_YUV
        fp_yuv = fopen("D:/output.yuv", "wb+");

        img_convert_ctx
                = sws_getContext
                    (
                    p_codec_context->width,
                    p_codec_context->height,
                    p_codec_context->pix_fmt,
                    640,
                    480,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC,
                    NULL,
                    NULL,
                    NULL
                    );
#else
        img_convert_ctx
                = sws_getContext
                    (
                    p_codec_context->width,
                    p_codec_context->height,
                    p_codec_context->pix_fmt,
                    DEFAULT_WIDTH,
                    DEFAULT_HEIGHT,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC,
                    NULL,
                    NULL,
                    NULL
                    );
#endif

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

    void GdiGrab::run()
    {
        ScreenShot();
    }

    void GdiGrab::ScreenShot()
    {
        while (m_shut_down)
        {
            if (av_read_frame(p_format_context, p_packet) >= 0)// 读取码流中的音频若干帧或者视频一帧,作为packet
            {
                if (p_packet->stream_index == videoindex)// 如果是视频
                {
                    int got_picture;
                    // 解码一帧视频数据。输入一个压缩编码的结构体AVPacket，输出一个解码后的结构体AVFrame
                    int ret = avcodec_decode_video2(p_codec_context, p_frame, &got_picture, p_packet);

                    if (ret < 0)
                    {
                        // 抛异常到界面层，让界面层处理
                        ThrowError(QString("Decode Error."));

                        Logger::LogMessage* log
                            = new Logger::LogMessage
                                (
                                Logger::INFO,
                                objectName().toStdString(),
                                "decode error",
                                QDateTime::currentDateTime().toString("yyyy-M-d h:m:s:zzz").toStdString()
                                );
                        DataBufferPointer::GetInstance().GetLog()->PushBack(std::shared_ptr<Logger::LogMessage>(log));
                    }

                    if (got_picture)
                    {
#ifdef OUT_FILE_YUV
                        uint8_t* buff = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, 640, 480));
                        avpicture_fill((AVPicture*)p_frame_yuv, buff, AV_PIX_FMT_YUV420P, 640, 480);
                        p_frame_yuv->linesize[0] = 640;
                        p_frame_yuv->linesize[1] = 640 / 2;
                        p_frame_yuv->linesize[2] = 640 / 2;
#else

                        // 相当于给pFrameYUV->data里面装的指针分配空间
                        // 申请空间来存放图片数据。包含源数据和目标数据
                        uint8_t* buff = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, DEFAULT_WIDTH, DEFAULT_HEIGHT));
                        // 前面的av_frame_alloc函数，只是为这个AVFrame结构体分配了内存，而data数组里面的指针指向的内存还没分配，这里把av_malloc得到的内存和AVFrame关联起来
                        avpicture_fill((AVPicture*)p_frame_yuv, buff, AV_PIX_FMT_YUV420P, DEFAULT_WIDTH, DEFAULT_HEIGHT);
                        p_frame_yuv->linesize[0] = DEFAULT_WIDTH;
                        p_frame_yuv->linesize[1] = DEFAULT_WIDTH / 2;
                        p_frame_yuv->linesize[2] = DEFAULT_WIDTH / 2;
                        p_frame_yuv->width = DEFAULT_WIDTH;
                        p_frame_yuv->height = DEFAULT_HEIGHT;
                        p_frame_yuv->format = AV_PIX_FMT_YUV420P;
#endif
                        //将输出结果转化成YUV
                        sws_scale(img_convert_ctx, (const unsigned char* const*)p_frame->data, p_frame->linesize, 0, p_codec_context->height, p_frame_yuv->data, p_frame_yuv->linesize);
#ifdef OUT_FILE_YUV
                        int y_size = 640 * 480;
                        fwrite(p_frame_yuv->data[0], 1, y_size, fp_yuv);    //Y
                        fwrite(p_frame_yuv->data[1], 1, y_size / 4, fp_yuv);  //U
                        fwrite(p_frame_yuv->data[2], 1, y_size / 4, fp_yuv);  //V
#endif
                        SaveData();
                        av_free(buff);
                    }
                }
                av_free_packet(p_packet);
            }
        }
    }

    void GdiGrab::SaveData()
    {
        // 将p_frame_yuv的裸数据以及基本信息拷贝到自定义Frame结构体中，并存入消息队列中去
        PicYUV* yuv_data = new PicYUV;
        yuv_data->width = p_frame_yuv->width;
        yuv_data->height = p_frame_yuv->height;

        for (int i = 0; i < yuv_data->NUM_DATA_POINTERS; ++i)
        {
            yuv_data->linesize[i] = p_frame_yuv->linesize[i];
        }

        int y_size = yuv_data->width * yuv_data->height;
        yuv_data->data[0] = new uint8_t[y_size];
        memcpy(yuv_data->data[0], p_frame_yuv->data[0], y_size);
        yuv_data->data[1] = new uint8_t[y_size / 4];
        memcpy(yuv_data->data[1], p_frame_yuv->data[1], y_size / 4);
        yuv_data->data[2] = new uint8_t[y_size / 4];
        memcpy(yuv_data->data[2], p_frame_yuv->data[2], y_size / 4);

        DataBufferPointer::GetInstance().GetPicYUVData()->PushBack(std::shared_ptr<PicYUV>(yuv_data));
    }
}
