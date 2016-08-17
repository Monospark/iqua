#include "openvr_system.h"

#include <stdexcept>

namespace
{
vr::IVRSystem* vrSystem = nullptr;
}

void OpenVrSystem::init()
{
    vr::EVRInitError eError = vr::VRInitError_None;
    vrSystem = vr::VR_Init(&eError, vr::VRApplication_Other);
    if (eError != vr::VRInitError_None) {
        char buf[1024];
        sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        throw std::runtime_error(buf);
    }
}

void OpenVrSystem::shutdown()
{
    vr::VR_Shutdown();
    vrSystem = nullptr;
}

vr::IVRSystem& OpenVrSystem::get()
{
    return *vrSystem;
}
