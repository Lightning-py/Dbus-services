
project(DBUSServices)

# Libraries
find_package(sdbus-c++ REQUIRED)
find_package(SQLite3 REQUIRED)


# Variables

# PermissionService
# Service
set(PERMISSION_SERVICE_DIR PermissionService)
set(PERMISSION_SERVICE_HEADERS ${PERMISSION_SERVICE_DIR}/headers)
set(PERMISSION_SERVICE_SOURCES ${PERMISSION_SERVICE_DIR}/sources)


# Client
set(PERMISSION_SERVICE_CLIENT_DIR PermissionServiceClient)
set(PERMISSION_SERVICE_CLIENT_HEADERS ${PERMISSION_SERVICE_CLIENT_DIR}/headers)


# SystemTimeService
# Service
set(SYSTEMTIME_SERVICE_DIR SystemTimeService)
set(SYSTEMTIME_SERVICE_HEADERS ${SYSTEMTIME_SERVICE_DIR}/headers)
set(SYSTEMTIME_SERVICE_SOURCES ${SYSTEMTIME_SERVICE_DIR}/sources)


# Client
set(SYSTEMTIME_SERVICE_CLIENT_DIR SystemTimeServiceClient)
set(SYSTEMTIME_SERVICE_CLIENT_HEADERS ${SYSTEMTIME_SERVICE_CLIENT_DIR}/headers)


# Executables
# PermissionService
# Service
add_executable(PermissionService ${PERMISSION_SERVICE_SOURCES}/main.cpp)
target_include_directories(PermissionService PUBLIC ${PERMISSION_SERVICE_HEADERS})
target_link_libraries(PermissionService PRIVATE SDBusCpp::sdbus-c++ sqlite3)



