#pragma once

#include <client_driver.h>
#include <openvr/openvr_driver.h>

class VreClientDriver final : public ClientDriver
{
    vr::EVRInitError onInit(vr::EClientDriverMode driverMode) override;
    bool isHmdPresent() override;
};
