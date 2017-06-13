#include "Logger.h"
#include "PicYUV.h"
#include "DataToUI.h"
#include "PicSendUI.h"

#include <QImage>

#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

namespace Display
{
    PicSendUI::PicSendUI(const int& width, const int& height) :
        DEFAULT_WIDTH(width),
        DEFAULT_HEIGHT(height)
    {
        setObjectName("PicSendUI");

        p_frame_yuv = NULL;
        p_frame_rgb = NULL;
        img_convert_ctx = NULL;
    }

    PicSendUI::~PicSendUI()
    {
        sws_freeContext(img_convert_ctx);
        av_free(p_frame_rgb);
        av_free(p_frame_yuv);

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

    bool PicSendUI::Initial()
    {
        p_frame_rgb = av_frame_alloc();
        p_frame_yuv = av_frame_alloc();

        img_convert_ctx
            = sws_getContext
                (
                DEFAULT_WIDTH,
                DEFAULT_HEIGHT,
                AV_PIX_FMT_YUV420P,
                DEFAULT_WIDTH,
                DEFAULT_HEIGHT,
                AV_PIX_FMT_RGB32,
                SWS_BICUBIC,
                NULL,
                NULL,
                NULL
                );

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

    void PicSendUI::run()
    {
        while (m_shut_down)
        {
            if (DataBufferPointer::GetInstance().GetPicYUVData()->HaveData(ClassName()))
            {
                std::shared_ptr<PicYUV> yuv_data = DataBufferPointer::GetInstance().GetPicYUVData()->PopTop(ClassName());
                if (yuv_data->data)
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    uint8_t* yuv_buff = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, DEFAULT_WIDTH, DEFAULT_HEIGHT));
                    avpicture_fill((AVPicture*)p_frame_yuv, yuv_buff, AV_PIX_FMT_YUV420P, DEFAULT_WIDTH, DEFAULT_HEIGHT);

                    uint8_t* out_buffer = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_RGB32, DEFAULT_WIDTH, DEFAULT_HEIGHT));
                    avpicture_fill((AVPicture*)p_frame_rgb, out_buffer, AV_PIX_FMT_RGB32, DEFAULT_WIDTH, DEFAULT_HEIGHT);

                    p_frame_yuv->width = yuv_data->width;
                    p_frame_yuv->height = yuv_data->height;
                    p_frame_yuv->format = AV_PIX_FMT_YUV420P;

                    for (int i = 0; i < PicYUV::NUM_DATA_POINTERS; ++i)
                    {
                        p_frame_yuv->linesize[i] = yuv_data->linesize[i];
                    }

                    int size = yuv_data->width * yuv_data->height;
                    memcpy(p_frame_yuv->data[0], yuv_data->data[0], size);
                    memcpy(p_frame_yuv->data[1], yuv_data->data[1], size / 4);
                    memcpy(p_frame_yuv->data[2], yuv_data->data[2], size / 4);

                    sws_scale(img_convert_ctx, (uint8_t const * const *)p_frame_yuv->data, p_frame_yuv->linesize, 0, DEFAULT_HEIGHT, p_frame_rgb->data, p_frame_rgb->linesize);

                    // 把这个RGB数据 用QImage加载
                    QImage _temp_image((uchar*)out_buffer, DEFAULT_WIDTH, DEFAULT_HEIGHT, QImage::Format_RGB32);
                    QImage image = _temp_image.copy();// 把图像复制一份 传递给界面显示
                    ShowImage(image);
                    av_free(yuv_buff);
                    av_free(out_buffer);
                }
                else
                {
                    goto Jump;
                }
            }
            else
            {
                Jump :
                    continue;
            }
            msleep(1);
        }
    }
}
