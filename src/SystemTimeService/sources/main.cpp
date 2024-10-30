#include "../headers/SystemTimeService.h"

int main()
{
    sdbus::ServiceName serviceName{"com.system.time"};
    auto connection = sdbus::createSessionBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/com/system/time"};

    try {
        SystemTimeService manager(*connection, std::move(objectPath),
                                  "log_time.txt");
        connection->enterEventLoop();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}