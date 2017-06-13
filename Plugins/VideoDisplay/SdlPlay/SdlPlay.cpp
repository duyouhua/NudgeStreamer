#include "PicYUV.h"
#include "SdlPlay.h"

#include <QWidget>

#ifdef OUT_FILE_YUV_E
#include <iostream>
#endif

extern "C"
{
#include <SDL.h>
}

namespace Display
{
    const int SFM_REFRESH_EVENT = SDL_USEREVENT + 1;
    SDL_Surface* pg_sdl_surface;// SDL_Surface对象，作为一块graphical memory存在，用于进行绘制

    SdlPlay::SdlPlay(QWidget* widget, const int& width, const int& height) :
        p_display_widget(widget),
        m_width(width),
        m_height(height)
    {
    }

    SdlPlay::~SdlPlay()
    {
#ifdef OUT_FILE_YUV_E
        fclose(fp_yuv);
#endif
    }

    void SdlPlay::Initial()
    {
        CreateSDLWindow();
    }

    // 把一个QT的窗口交给SDL使用
    void SdlPlay::CreateSDLWindow()
    {
        char winID[256] = { 0 };
        // 获取窗口的大小
        QSize size = p_display_widget->baseSize();
        sprintf(winID, "SDL_WINDOWID=0x%lx", (long unsigned int)p_display_widget->winId());
        SDL_putenv(winID);

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            ThrowError(QString("Can not initialize SDL."));
        }

        pg_sdl_surface = SDL_SetVideoMode(size.width(), size.height(), 0, SDL_HWSURFACE | SDL_ANYFORMAT);
        if (!pg_sdl_surface)
        {
            ThrowError(QString("SDL can not set vide mode."));
        }
    }

    int SdlPlay::RefreshThread(void* opaque)
    {
        while (true)
        {
            SDL_Event event;
            event.type = SFM_REFRESH_EVENT;
            SDL_PushEvent(&event);
            SDL_Delay(42);
        }

        return 0;
    }

    void SdlPlay::run()
    {
        SDL_Overlay* yuv = SDL_CreateYUVOverlay(m_width, m_height, SDL_YUY2_OVERLAY, pg_sdl_surface);
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = p_display_widget->width();
        rect.h = p_display_widget->height();

        SDL_Thread* video_tid = SDL_CreateThread(RefreshThread, NULL);
        // Event Loop
        SDL_Event event;

        int refresh = 1;
        int finish = 1;

#ifdef OUT_FILE_YUV_E
        fp_yuv = fopen("E:/YUV_output.yuv", "wb+");
#endif

        while (m_shut_down)
        {
            // Wait
            SDL_WaitEvent(&event);
            if (SFM_REFRESH_EVENT == event.type)
            {
                if (DataBufferPointer::GetInstance().GetPicYUVData()->HaveData(ClassName()))
                {
                    std::shared_ptr<PicYUV> data_yuv = DataBufferPointer::GetInstance().GetPicYUVData()->PopTop(ClassName());
                    if (data_yuv->data)
                    {
#ifdef OUT_FILE_YUV_E
                        int y_size = picture->width * picture->height;
                        fwrite(picture->data[0], 1, y_size, fp_yuv);
                        fwrite(picture->data[1], 1, y_size / 4, fp_yuv);
                        fwrite(picture->data[2], 1, y_size / 4, fp_yuv);
#else
                        SDL_LockYUVOverlay(yuv);
                        yuv->pixels[0] = data_yuv->data[0];
                        yuv->pixels[1] = data_yuv->data[2];
                        yuv->pixels[2] = data_yuv->data[1];
                        yuv->pitches[0] = data_yuv->linesize[0];
                        yuv->pitches[1] = data_yuv->linesize[2];
                        yuv->pitches[2] = data_yuv->linesize[1];
                        SDL_UnlockYUVOverlay(yuv);
                        SDL_DisplayYUVOverlay(yuv, &rect);
#endif
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
            }
            msleep(1);
        }
        SDL_FreeYUVOverlay(yuv);
        SDL_FreeSurface(pg_sdl_surface);
    }
}
