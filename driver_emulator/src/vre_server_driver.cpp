#include "vre_server_driver.h"

#include "logger.h"
#include "hmd.h"
#include "display_component.h"
#include "tracking_reference.h"
#include "hmd_mock.h"

std::unique_ptr<VreServerDriver> VreServerDriver::instance;

const std::unique_ptr<VreServerDriver>& VreServerDriver::getInstance()
{
    return instance;
}

vr::EVRInitError VreServerDriver::onInit(std::vector<std::unique_ptr<TrackedDevice>>& devices)
{
    vr::DriverPose_t pose{};
    pose.deviceIsConnected = true;
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;

    TrackedDevice::Data hmdBaseData{};
    hmdBaseData.serialNumber = "hmd";
    hmdBaseData.modelNumber = "hmd";
    hmdBaseData.deviceClass = vr::TrackedDeviceClass_HMD;
    Hmd::Data hmdData{};
    hmdData.currentUniverseId = 1464782634;
    hmdData.ipdMeters = 0.65f;
    hmdData.userHeadToEyeDepthMeters = 0.0f;
    hmdData.displayFrequency = 90;
    hmdData.secondsFromVsyncToPhotons = 0.0f;
    hmdData.onDesktop = false;
    std::vector<std::shared_ptr<Component>> hmdComponents{ std::make_shared<DisplayComponent>(0, 0, 1920, 1080, true, true, 1920, 1080) };
    devices.push_back(std::make_unique<HmdMock>(hmdComponents, hmdBaseData, hmdData));

    TrackedDevice::Data trackRefBaseData1{};
    trackRefBaseData1.serialNumber = "trackref1";
    trackRefBaseData1.modelNumber = "trackref1";
    trackRefBaseData1.deviceClass = vr::TrackedDeviceClass_TrackingReference;

    TrackedDevice::Data trackRefBaseData2{};
    trackRefBaseData2.serialNumber = "trackref2";
    trackRefBaseData2.modelNumber = "trackref2";
    trackRefBaseData2.deviceClass = vr::TrackedDeviceClass_TrackingReference;

    devices.push_back(std::make_unique<TrackingReference>(std::vector<std::shared_ptr<Component>>{}, trackRefBaseData1, TrackingReference::Data{}, pose));
    devices.push_back(std::make_unique<TrackingReference>(std::vector<std::shared_ptr<Component>>{}, trackRefBaseData2, TrackingReference::Data{}, pose));

    //Window::openWindow(this);
    instance = std::unique_ptr<VreServerDriver>(this);
    return vr::VRInitError_None;
}

void VreServerDriver::onShutdown()
{
    instance.release();
}
