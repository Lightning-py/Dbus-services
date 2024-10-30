#include "PermissionService.h"

int main()
{
    sdbus::ServiceName serviceName{"com.system.permissions"};
    auto connection = sdbus::createSessionBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/com/system/permissions"};

    try {
        PermissionManager manager(*connection, std::move(objectPath), "log.txt",
                                  "database.db");
        connection->enterEventLoop();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}