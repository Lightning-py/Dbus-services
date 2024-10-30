#include "../headers/SystemTimeService.h"

int main(int argc, char *argv[])
{
    sdbus::ServiceName serviceName{"com.system.time"};
    auto connection = sdbus::createBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/com/system/time"};

    try {
        SystemTimeService manager(*connection, std::move(objectPath),
                                  "log_time.txt");
        connection->enterEventLoop();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}