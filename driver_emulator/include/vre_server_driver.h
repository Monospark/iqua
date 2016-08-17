#pragma once

#include <openvr/openvr_driver.h>
#include <memory>
#include <vector>
#include <tracked_device.h>
#include <server_driver.h>
#include "vre_api.h"

class VreServerDriver final : public ServerDriver
{
public:
    static VRE_API const std::unique_ptr<VreServerDriver>& getInstance();

private:
    static std::unique_ptr<VreServerDriver> instance;

    vr::EVRInitError onInit(std::vector<std::unique_ptr<TrackedDevice>>& devices) override;
    void onShutdown() override;
};
