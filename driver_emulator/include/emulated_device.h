#pragma once

struct Pose;

class EmulatedDevice
{
public:
    virtual ~EmulatedDevice();
    virtual void setPose(const Pose& pose) = 0;
};
