#include <unistd.h>

#include <iostream>

#include "PermissionClient.h"
#include "SystemTimeServiceClient.h"

void requestPermission()
{
    sdbus::ServiceName destination_perm{"com.system.permissions"};
    sdbus::ObjectPath objectPath_perm{"/com/system/permissions"};
    PermissionClient permissionclient(std::move(destination_perm),
                                      std::move(objectPath_perm));
    permissionclient.RequestPermission(0);
}

int main(int argc, char *argv[])
{
    sdbus::ServiceName destination{"com.system.time"};
    sdbus::ObjectPath objectPath{"/com/system/time"};
    SystemTimeServiceClient client(std::move(destination),
                                   std::move(objectPath));

    try {
        uint64_t result = client.GetSystemTime();
        std::cout << result << std::endl;
    } catch (const sdbus::Error &error) {
        if (error.getName() == "com.system.UnauthorizedAccess") {
            std::cerr << "Access denied!" << std::endl;
            std::cerr << error.what() << std::endl;
            std::cout << "Trying to get permission" << std::endl;

            requestPermission();

            std::cout << "Got a permission. Trying to get "
                         "timestamp again."
                      << std::endl;
            try {
                uint64_t result = client.GetSystemTime();
                std::cout << result << std::endl;
            } catch (const std::exception &e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        } else {
            std::cout << "Some kind of dbus error: " << error.getName()
                      << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    sleep(1);

    return 0;
}