#ifndef SYSTEMTIMESERVICECLIENT_H
#define SYSTEMTIMESERVICECLIENT_H

#include <sdbus-c++/sdbus-c++.h>

#include "time_adaptor_client.h"

class SystemTimeServiceClient
    : public sdbus::ProxyInterfaces<com::system::time_proxy>
{
   public:
    SystemTimeServiceClient(sdbus::ServiceName destination,
                            sdbus::ObjectPath objectPath)
        : ProxyInterfaces(std::move(destination), std::move(objectPath))
    {
        registerProxy();
    }

    ~SystemTimeServiceClient() { unregisterProxy(); }
};

#endif  // SYSTEMTIMESERVICECLIENT_H
