/*
 * 该类实现视频显示模块，继承VidoeDisplay抽象类，
 * 它从消息队列中获取截屏后的YUV格式图片，利用ffm
 * peg库对其转换成RBA格式，存到QImage对象中，再
 * 通过中间件DataToUI对象将QImage发送到界面层进
 * 行显示
 */

#ifndef _PIC_SEND_UI_H
#define _PIC_SEND_UI_H

#include "VideoDisplay.h"
#include <mutex>

struct AVFrame;
struct SwsContext;

namespace Display
{
    class PicSendUI : public VideoDisplay
    {
    public:
        PicSendUI(const int& width, const int& height);
        ~PicSendUI();

    public:
        bool Initial();

    protected:
        void run();

    private:
        // 截屏后保存的图片宽和高
        const int DEFAULT_WIDTH;
        const int DEFAULT_HEIGHT;

    private:
        AVFrame* p_frame_yuv;
        AVFrame* p_frame_rgb;
        struct SwsContext* img_convert_ctx;
        std::mutex m_mutex;
    };
}

#endif // _PIC_SEND_UI_H
