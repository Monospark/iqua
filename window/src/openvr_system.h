#pragma once

#include <openvr.h>

namespace OpenVrSystem
{
void init();
void shutdown();
vr::IVRSystem& get();
};

