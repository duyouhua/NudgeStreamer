/*
 * GdiGrab类主要运用ffmpeg库调用Windows Gdi进行截屏；
 * 截屏后运用ffmpeg库对RGB的数据转化为YUV格式的数据；
 * 转化后数据存在ffmpeg库中的AVFrame结构体，则需要将
 * 其数据拷贝到自定义Frame结构体中；
 */

#ifndef _GDI_GRAB_H
#define _GDI_GRAB_H

#include "InterceptScreen.h"

struct AVCodec;
struct AVFrame;
struct AVPacket;
struct SwsContext;
struct AVCodecContext;
struct AVFormatContext;

#ifdef OUT_FILE_YUV
struct FILE;
#endif

namespace Catpure
{
    class GdiGrab : public InterceptScreen
    {
    public:
        GdiGrab(const int& width, const int& height);
        ~GdiGrab();

    public:
        bool Initial();

    protected:
        void run();

    private:
        void ScreenShot();
        void SaveData();

    private:
        // 默认截屏后保存图片的宽度以及高度
        const int DEFAULT_WIDTH;
        const int DEFAULT_HEIGHT;

    private:
        AVFormatContext* p_format_context;
        AVCodecContext* p_codec_context;
        AVCodec* p_codec;
        AVPacket* p_packet;
        struct SwsContext* img_convert_ctx;
        AVFrame* p_frame;
        AVFrame* p_frame_yuv;
        int videoindex;
#ifdef OUT_FILE_YUV
        FILE* fp_yuv;
#endif
    };
}

#endif // _GDI_GRAB_H
