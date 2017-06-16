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

        _format_context = NULL;
        _codec_context = NULL;
        _codec = NULL;
        _frame = NULL;
        _frame_yuv = NULL;
        _img_convert_ctx = NULL;
    }

    GdiGrab::~GdiGrab()
    {
#ifdef OUT_FILE_YUV
        fclose(fp_yuv);
#endif
        sws_freeContext(_img_convert_ctx);
        av_free(_frame_yuv);
        av_free(_frame);
        avcodec_close(_codec_context);
        avformat_close_input(&_format_context);

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
        _format_context = avformat_alloc_context();

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
        if (avformat_open_input(&_format_context, "desktop", ifmt, &options) != 0)
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

        if (avformat_find_stream_info(_format_context, NULL) < 0)
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

        for (int i = 0; i < _format_context->nb_streams; i++)
        {
            if (_format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                video_index = i;
                break;
            }
        }

        if (video_index == -1)
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

        _codec_context = _format_context->streams[video_index]->codec;
        _codec = avcodec_find_decoder(_codec_context->codec_id);

        if (_codec == NULL)
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

        if (avcodec_open2(_codec_context, _codec, NULL) < 0)
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

        _frame = av_frame_alloc();
        _frame_yuv = av_frame_alloc();

        _packet = (AVPacket*)av_malloc(sizeof(AVPacket));
#ifdef OUT_FILE_YUV
        fp_yuv = fopen("D:/output.yuv", "wb+");

        _img_convert_ctx
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
        _img_convert_ctx
                = sws_getContext
                    (
                    _codec_context->width,
                    _codec_context->height,
                    _codec_context->pix_fmt,
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
        while (shut_down)
        {
            if (av_read_frame(_format_context, _packet) >= 0)// 读取码流中的音频若干帧或者视频一帧,作为packet
            {
                if (_packet->stream_index == video_index)// 如果是视频
                {
                    int got_picture;
                    // 解码一帧视频数据。输入一个压缩编码的结构体AVPacket，输出一个解码后的结构体AVFrame
                    int ret = avcodec_decode_video2(_codec_context, _frame, &got_picture, _packet);

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
                        avpicture_fill((AVPicture*)_frame_yuv, buff, AV_PIX_FMT_YUV420P, DEFAULT_WIDTH, DEFAULT_HEIGHT);
                        _frame_yuv->linesize[0] = DEFAULT_WIDTH;
                        _frame_yuv->linesize[1] = DEFAULT_WIDTH / 2;
                        _frame_yuv->linesize[2] = DEFAULT_WIDTH / 2;
                        _frame_yuv->width = DEFAULT_WIDTH;
                        _frame_yuv->height = DEFAULT_HEIGHT;
                        _frame_yuv->format = AV_PIX_FMT_YUV420P;
#endif
                        //将输出结果转化成YUV
                        sws_scale(_img_convert_ctx, (const unsigned char* const*)_frame->data, _frame->linesize, 0, _codec_context->height, _frame_yuv->data, _frame_yuv->linesize);
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
                av_free_packet(_packet);
            }
        }
    }

    void GdiGrab::SaveData()
    {
        // 将p_frame_yuv的裸数据以及基本信息拷贝到自定义Frame结构体中，并存入消息队列中去
        PicYUV* yuv_data = new PicYUV;
        yuv_data->width = _frame_yuv->width;
        yuv_data->height = _frame_yuv->height;

        for (int i = 0; i < yuv_data->NUM_DATA_POINTERS; ++i)
        {
            yuv_data->linesize[i] = _frame_yuv->linesize[i];
        }

        int y_size = yuv_data->width * yuv_data->height;
        yuv_data->data[0] = new uint8_t[y_size];
        memcpy(yuv_data->data[0], _frame_yuv->data[0], y_size);
        yuv_data->data[1] = new uint8_t[y_size / 4];
        memcpy(yuv_data->data[1], _frame_yuv->data[1], y_size / 4);
        yuv_data->data[2] = new uint8_t[y_size / 4];
        memcpy(yuv_data->data[2], _frame_yuv->data[2], y_size / 4);

        DataBufferPointer::GetInstance().GetPicYUVData()->PushBack(std::shared_ptr<PicYUV>(yuv_data));
    }
}
