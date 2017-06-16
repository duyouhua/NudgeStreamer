//#include "SdlPlay.h"
#include "DataToUI.h"
#include "PicSendUI.h"
#include "DisplayFactory.h"

namespace Display
{
    DisplayFactory::DisplayFactory()
    {
    }

    DisplayFactory::~DisplayFactory()
    {
    }

    VideoDisplay* DisplayFactory::CreateDisplayClass(const std::string& type, const int& width, const int& height, QWidget* widget)
    {
        if ("" == type)
        {
            DataToUI::GetInstance().setParament(QString("Can not to initialize Display Lib."));
            return NULL;
        }

        if ("PicSendUI" == type && !widget)
        {
            return new PicSendUI(width, height);
        }

//        if ("SDL" == type && widget)
//        {
//            return new SdlPlay(widget, width, height);
//        }
    }
}
