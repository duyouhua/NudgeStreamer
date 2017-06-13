/*
 * 运用SDL库来进行截屏后的界面显示，
 * 初始化该对象时将会传入一个视频播
 * 放控件的指针，从而获得窗口句柄，
 * 使得SDL可完全控制该控件进行视频
 * 播放
 */

#ifndef _SDL_PLAY_H
#define _SDL_PLAY_H

#include "VideoDisplay.h"

#ifdef OUT_FILE_YUV_E
struct FILE;
#endif

class QWidget;
struct SDL_Surface;

namespace Display
{
    extern const int SFM_REFRESH_EVENT;
    extern SDL_Surface* pg_sdl_surface;

    class SdlPlay : public VideoDisplay
    {

    public:
        SdlPlay(QWidget* widget, const int& width, const int& height);
        ~SdlPlay();

    public:
        void Initial();

    protected:
        void run();

    private:
        void CreateSDLWindow();// 把一个QT的窗口交给SDL使用
        static int RefreshThread(void* opaque);// Refresh Event

    private:
        int m_width;
        int m_height;
        QWidget* p_display_widget;
#ifdef OUT_FILE_YUV_E
        FILE* fp_yuv;
#endif
    };
}

#endif // _SDL_PLAY_H
