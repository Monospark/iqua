#pragma once

#include "hmd.h"
#include "pose.h"
#include <emulated_device.h>

class HmdMock : public Hmd, public EmulatedDevice
{
public:
    HmdMock(
        const std::vector<std::shared_ptr<Component>>& components,
        const TrackedDevice::Data& trackedDeviceData,
        const Hmd::Data& hmdData);

    void setPose(const Pose& pose) override;

private:
    bool poseChanged;

    bool update() override;
};
