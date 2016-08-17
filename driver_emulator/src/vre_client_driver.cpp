#include "vre_client_driver.h"

#include <logger.h>

vr::EVRInitError VreClientDriver::onInit(vr::EClientDriverMode driverMode)
{
    if (driverMode == vr::EClientDriverMode::ClientDriverMode_Watchdog) {
        Logger::get().log("Watchdog mode was requested but is not available");
        return vr::VRInitError_Init_NotAvailableToWatchdogApps;
    }

    return vr::VRInitError_None;
}

bool VreClientDriver::isHmdPresent()
{
    return true;
}
