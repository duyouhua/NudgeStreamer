#include "TcpTest.h"
#include "DataToUI.h"
#include "NetworkFactory.h"

namespace Network
{
    NetworkFactory::NetworkFactory()
    {
    }

    NetworkFactory::~NetworkFactory()
    {
    }

    NetworkQuality* NetworkFactory::CreateNetworkClass(const std::string& type, const std::string& ip, const unsigned short& port)
    {
        if ("" == type)
        {
            DataToUI::GetInstance().setParament(QString("Can not to initialize Network Lib."));
        }

        if ("Tcping" == type)
        {
            return new TcpTest(ip, port);
        }
    }
}
