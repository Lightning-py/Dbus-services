#ifndef PERMISSIONCLIENT_H
#define PERMISSIONCLIENT_H

#include <sdbus-c++/sdbus-c++.h>

#include "PermissionServiceClient.h"

class PermissionClient
    : public sdbus::ProxyInterfaces<com::system::permissions_proxy>
{
   public:
    PermissionClient(sdbus::ServiceName destination,
                     sdbus::ObjectPath objectPath)
        : ProxyInterfaces(sdbus::createSessionBusConnection(),
                          std::move(destination), std::move(objectPath))
    {
        registerProxy();
    }

    ~PermissionClient() { unregisterProxy(); }
};

#endif  // PERMISSIONCLIENT_H
