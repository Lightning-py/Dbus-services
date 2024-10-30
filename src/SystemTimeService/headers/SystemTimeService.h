#ifndef SYSTEMTIMESERVICE_H
#define SYSTEMTIMESERVICE_H

#include <sdbus-c++/sdbus-c++.h>
#include <unistd.h>

#include <chrono>
#include <climits>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "PermissionClient.h"
#include "time_adaptor.h"

enum Permissions { SystemTime = 0 };

class SystemTimeService
    : public sdbus::AdaptorInterfaces<com::system::time_adaptor>
{
   public:
    SystemTimeService(sdbus::IConnection &connection,
                      sdbus::ObjectPath objectPath, std::string &&logfile)
        : AdaptorInterfaces(connection, std::move(objectPath)),
          LOGFILE_PATH(logfile)
    {
        try {
            registerAdaptor();
            write_log("Service started");
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

    ~SystemTimeService()
    {
        try {
            unregisterAdaptor();
            write_log("Service stopped");
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

   private:
    std::string LOGFILE_PATH;

   protected:
    std::string getCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        struct tm *parts = std::localtime(&now_c);

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "[ %Y-%m-%d %H:%M:%S ]", parts);
        return std::string(buffer);
    }

    std::string getExecutablePath(pid_t pid)
    {
        char path[PATH_MAX];
        char buffer[PATH_MAX];
        snprintf(path, sizeof(path), "/proc/%d/exe", pid);
        ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);

        if (len != -1) {
            buffer[len] = '\0';
            return std::string(buffer);
        }

        return "";
    }

    std::string getClientExecutable(const std::string &clientBusName)
    {
        try {
            sdbus::ServiceName destination{"org.freedesktop.DBus"};
            sdbus::ObjectPath objectPath{"/org/freedesktop/DBus"};
            auto proxy = sdbus::createProxy(std::move(destination),
                                            std::move(objectPath));

            uint32_t pid;
            proxy->callMethod("GetConnectionUnixProcessID")
                .onInterface("org.freedesktop.DBus")
                .withArguments(clientBusName)
                .storeResultsTo(pid);

            return getExecutablePath(pid);

        } catch (const sdbus::Error &e) {
            std::cerr << "D-Bus error: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        return "";
    }

    void write_log(std::string message)
    {
        std::ofstream file;
        file.open(LOGFILE_PATH, std::ios::app);

        if (file.is_open()) {
            file << getCurrentTime() << "\t" << message << "\n";
            file.close();
        } else {
            std::cerr << "Error while opening a log file with "
                         "following address:\t"
                      << LOGFILE_PATH << std::endl;
        }
    }

    void write_getsystemtime_log(std::string clientExecutable, bool result)
    {
        std::string message =
            "Got a request from client with executable: " + clientExecutable +
            ". Permission check: " + std::to_string(result);
        write_log(message);
    }

    uint64_t GetSystemTime()
    {
        sdbus::ServiceName destination{"com.system.permissions"};
        sdbus::ObjectPath objectPath{"/com/system/permissions"};
        PermissionClient permissionclient(std::move(destination),
                                          std::move(objectPath));

        std::string clientBusName =
            getObject().getCurrentlyProcessedMessage().getSender();
        std::string clientExecutable = getClientExecutable(clientBusName);

        bool result = permissionclient.CheckApplicationHasPermission(
            clientExecutable, SystemTime);
        write_getsystemtime_log(clientExecutable, result);

        if (result) {
            auto now = std::chrono::system_clock::now();
            std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
            return timestamp;
        }
        throw sdbus::Error(sdbus::Error::Name{"com.system.UnauthorizedAccess"},
                           "Permission check failed");
    }
};

#endif  // SYSTEMTIMESERVICE_H
