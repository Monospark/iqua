#include "hmd_mock.h"

#include <openvr/openvr_driver.h>
#include <logger.h>

HmdMock::HmdMock(
    const std::vector<std::shared_ptr<Component>>& components,
    const TrackedDevice::Data& trackedDeviceData,
    const Hmd::Data& hmdData)
    : Hmd(components, trackedDeviceData, hmdData)
    , poseChanged(true)
{
    pose.deviceIsConnected = true;
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
}

void HmdMock::setPose(const Pose& newPose) {
    pose.vecPosition[0] = newPose.position.x;
    pose.vecPosition[1] = newPose.position.y;
    pose.vecPosition[2] = newPose.position.z;

    Logger::get().log("Pos change %f %f %f", newPose.position.x, newPose.position.y, newPose.position.z);

    //pose.qRotation.x = newPose.rotation.x;
    //pose.qRotation.y = newPose.rotation.y;
    //pose.qRotation.z = newPose.rotation.z;
    //pose.qRotation.w = newPose.rotation.w;
    poseChanged = true;
}

bool HmdMock::update() {
    if (poseChanged) {
        poseChanged = false;
        return true;
    }
    return false;
}

