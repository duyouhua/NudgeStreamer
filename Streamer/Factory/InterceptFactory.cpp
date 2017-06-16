#include "GdiGrab.h"
#include "DataToUI.h"
#include "InterceptFactory.h"

namespace Catpure
{
    InterceptFactory::InterceptFactory()
    {
    }

    InterceptFactory::~InterceptFactory()
    {
    }

    InterceptScreen* InterceptFactory::CreateInterceptClass(const std::string& type, const int& width, const int& height)
    {
        if ("" == type)
        {
            DataToUI::GetInstance().setParament(QString("Can not to initialize InterceptScreen Lib."));
            return NULL;
        }

        if ("GdiGrab" == type)
        {
            return new GdiGrab(width, height);
        }
    }
}
