#include "controller.h"

#include <windows.h>
#include "mathhelper.h"
#include <emulated_device.h>
#include <vre_server_driver.h>

namespace
{
const float MOUSE_SENSITIVITY = 0.1f;
}

Controller::Controller() : updater(this)
{
    connect(&updater, SIGNAL(timeout()), this, SLOT(updateInput()));
    updater.start();
}

Controller::~Controller() {}

void Controller::moveMouse(int32_t x, int32_t y) {
    rotationX += y * MOUSE_SENSITIVITY;
    rotationY += x * MOUSE_SENSITIVITY;

    Pose::Quaternion rotXQuat = MathHelper::axisAngleToQuaternion(1.0f, 0.0f, 0.0f, rotationX);
    Pose::Quaternion rotYQuat = MathHelper::axisAngleToQuaternion(1.0f, 0.0f, 0.0f, rotationY);
    pose.rotation = MathHelper::multiplyQuaternions(rotXQuat, rotYQuat);
    updatePose();
}

void Controller::updateInput() {
    qint64 elapsedTime = updateTimer.elapsed();
    float deltaTime = elapsedTime / 1000.0f;

    float xMove = 0.0f;
    if (GetKeyState('D')) {
        xMove += 1.0f;
    }
    if (GetKeyState('A')) {
        xMove -= 1.0f;
    }

    float zMove = 0.0f;
    if (GetKeyState('W')) {
        zMove += 1.0f;
    }
    if (GetKeyState('S')) {
        zMove -= 1.0f;
    }

    if (xMove == 0.0f && zMove == 0.0f)
    {
        return;
    }

    float rotYRad = rotationY * MathHelper::DEGREES_TO_RADIANS;
    float sidewaysMove = ((cos(rotYRad) * xMove) + (sin(rotYRad) * zMove)) * deltaTime;
    float straightMove = ((sin(rotYRad) * xMove) + (cos(rotYRad) * zMove)) * deltaTime;
    pose.position.x += sidewaysMove;
    pose.position.z += straightMove;
    updatePose();
}

void Controller::updatePose() const
{
    EmulatedDevice* hmd = dynamic_cast<EmulatedDevice*>(VreServerDriver::getInstance()->getTrackedDeviceById(0));
    hmd->setPose(pose);
}
