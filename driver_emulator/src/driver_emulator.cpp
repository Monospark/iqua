#include <vre_server_driver.h>
#include "vre_client_driver.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

VreClientDriver clientDriver;
VreServerDriver serverDriver;

DLL_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
    if (0 == strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName))
    {
        return &serverDriver;
    }
    if (0 == strcmp(vr::IClientTrackedDeviceProvider_Version, pInterfaceName))
    {
        return &clientDriver;
    }

    if (pReturnCode)
    {
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }

    return nullptr;
}
