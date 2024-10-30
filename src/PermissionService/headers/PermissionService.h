#ifndef PERMISSION_SERVICE_H
#define PERMISSION_SERVICE_H

#include <sdbus-c++/sdbus-c++.h>
#include <unistd.h>

#include <chrono>
#include <climits>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Database.h"
#include "permissions_adapter.h"

enum Permissions { SystemTime = 0 };

class PermissionManager
    : public sdbus::AdaptorInterfaces<com::system::permissions_adaptor>
{
   public:
    PermissionManager(sdbus::IConnection &connection,
                      sdbus::ObjectPath objectPath, std::string &&logfile,
                      std::string &&db_path)
        : AdaptorInterfaces(connection, std::move(objectPath)),
          LOGFILE_PATH(logfile),
          DB_PATH(db_path),
          db(DB_PATH)
    {
        try {
            registerAdaptor();
            write_log("Service started");
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

    ~PermissionManager()
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
    std::string DB_PATH;
    AppPermissionsDB db;

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

    void write_requestpermission_log(std::string clientExecutablePath,
                                     int permissionEnumCode)
    {
        std::string message = "Client with executable path: \"" +
                              clientExecutablePath +
                              "\"  requested permission with enum code: " +
                              std::to_string(permissionEnumCode);

        write_log(message);
    }

    void write_checkpermission_log(std::string clientExecutablePath,
                                   int permissionEnumCode,
                                   std::string applicationExecutablePath,
                                   bool result)
    {
        std::string message =
            "Client with executable path: " + clientExecutablePath +
            " checked permission with enum code: " +
            std::to_string(permissionEnumCode) +
            " of executable: " + applicationExecutablePath +
            ". Result: " + std::to_string(result);
        write_log(message);
    }

    void RequestPermission(const int32_t &permissionEnumCode) override
    {
        std::string clientBusName =
            getObject().getCurrentlyProcessedMessage().getSender();
        std::string clientExecutable = getClientExecutable(clientBusName);

        write_requestpermission_log(clientExecutable, permissionEnumCode);

        db.addPermission(clientExecutable, permissionEnumCode);
    }

    bool CheckApplicationHasPermission(const std::string &ApplicationExecPath,
                                       const int32_t &PermissionEnumCode)
    {
        auto app = db.getApp(ApplicationExecPath);

        bool flag = false;
        for (int permission : app.second) {
            if (permission == PermissionEnumCode) {
                flag = true;
                break;
            }
        }

        std::string clientBusName =
            getObject().getCurrentlyProcessedMessage().getSender();
        std::string clientExecutable = getClientExecutable(clientBusName);

        write_checkpermission_log(clientExecutable, PermissionEnumCode,
                                  clientExecutable, flag);

        return flag;
    }
};

#endif  // PERMISSION_SERVICE_H
