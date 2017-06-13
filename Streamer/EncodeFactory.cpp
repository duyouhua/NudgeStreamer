#include "AvEncode.h"
#include "DataToUI.h"
#include "EncodeFactory.h"

namespace Encode
{
    EncodeFactory::EncodeFactory()
    {
    }

    EncodeFactory::~EncodeFactory()
    {
    }

    VideoEncode* EncodeFactory::CreateEncodeClass(const std::string& type, const int& width, const int& height)
    {
        if ("" == type)
        {
            DataToUI::GetInstance().setParament(QString("Can not to initialize VideoEncode Lib."));
            return NULL;
        }

        if ("AvEncode" == type)
        {
            return new AvEncode(width, height);
        }
    }
}
