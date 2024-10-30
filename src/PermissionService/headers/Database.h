#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class AppPermissionsDB
{
   private:
    sqlite3 *db{};
    std::string DATABASE_NAME;
    std::string TABLE_NAME;

   public:
    explicit AppPermissionsDB(const std::string &dbName,
                              std::string &&tableName = "app_permissions")
        : DATABASE_NAME(dbName), TABLE_NAME(tableName)
    {
        if (sqlite3_open(dbName.c_str(), &db)) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db)
                      << std::endl;
            return;
        }
        createTable();
    }

    ~AppPermissionsDB() { sqlite3_close(db); }

    void createTable() const
    {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + TABLE_NAME +
                          " (executable TEXT PRIMARY KEY, permissions TEXT)";
        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

    bool insertApp(const std::string &clientExecutable,
                   const std::vector<int> &permissions)
    {
        std::string permStr;
        for (size_t i = 0; i < permissions.size(); i++) {
            permStr += std::to_string(permissions[i]);
            if (i != permissions.size() - 1) permStr += " ";
        }

        std::string sql = "INSERT OR REPLACE INTO " + TABLE_NAME +
                          " (executable, permissions) VALUES (?, ?);";

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db)
                      << std::endl;
            return false;
        }

        const std::string executable = "\"" + clientExecutable + "\"";

        sqlite3_bind_text(stmt, 1, (executable).c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, permStr.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        return rc == SQLITE_DONE;
    }

    std::pair<std::string, std::vector<int>> getApp(const std::string &appName)
    {
        std::string sql = "SELECT * FROM " + TABLE_NAME +
                          " WHERE executable = " + "'\"" + appName + "\"';";

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db)
                      << std::endl;
            return {"", {}};
        }

        std::pair<std::string, std::vector<int>> result;
        int result_code = sqlite3_step(stmt);

        if (result_code == SQLITE_ROW) {
            result.first =
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            std::string permStr =
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

            std::istringstream iss(permStr);
            int perm;
            while (iss >> perm) { result.second.push_back(perm); }
        }

        sqlite3_finalize(stmt);
        return result;
    }

    bool addPermission(const std::string &appName, int newPermission)
    {
        auto app = getApp(appName);

        bool flag = true;
        for (int permission : app.second) {
            if (newPermission == permission) flag = false;
        }

        if (flag) {
            app.second.push_back(newPermission);
            return insertApp(appName, app.second);
        }

        return insertApp(appName, {newPermission});
    }
};

#endif  // DATABASE_H