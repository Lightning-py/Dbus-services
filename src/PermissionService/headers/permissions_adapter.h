
/*
 * This file was automatically generated by sdbus-c++-xml2cpp; DO NOT EDIT!
 */

#ifndef __sdbuscpp__PermissionService_h__adaptor__H__
#define __sdbuscpp__PermissionService_h__adaptor__H__

#include <sdbus-c++/sdbus-c++.h>

#include <string>

namespace com
{
namespace system
{

class permissions_adaptor
{
   public:
    static constexpr const char *INTERFACE_NAME = "com.system.permissions";

   protected:
    permissions_adaptor(sdbus::IObject &object) : m_object(object) {}

    permissions_adaptor(const permissions_adaptor &) = delete;
    permissions_adaptor &operator=(const permissions_adaptor &) = delete;
    permissions_adaptor(permissions_adaptor &&) = delete;
    permissions_adaptor &operator=(permissions_adaptor &&) = delete;

    ~permissions_adaptor() = default;

    void registerAdaptor()
    {
        m_object
            .addVTable(
                sdbus::registerMethod("RequestPermission")
                    .withInputParamNames("PermissionEnumCode")
                    .implementedAs([this](const int32_t &PermissionEnumCode) {
                        return this->RequestPermission(PermissionEnumCode);
                    }),
                sdbus::registerMethod("CheckApplicationHasPermission")
                    .withInputParamNames("ApplicationExecPath",
                                         "PermissionEnumCode")
                    .withOutputParamNames("answer")
                    .implementedAs(
                        [this](const std::string &ApplicationExecPath,
                               const int32_t &PermissionEnumCode) {
                            return this->CheckApplicationHasPermission(
                                ApplicationExecPath, PermissionEnumCode);
                        }))
            .forInterface(INTERFACE_NAME);
    }

   private:
    virtual void RequestPermission(const int32_t &PermissionEnumCode) = 0;
    virtual bool CheckApplicationHasPermission(
        const std::string &ApplicationExecPath,
        const int32_t &PermissionEnumCode) = 0;

   private:
    sdbus::IObject &m_object;
};

}  // namespace system
}  // namespace com

#endif