#pragma once

#include <pose.h>

namespace MathHelper
{
const float DEGREES_TO_RADIANS = 1.0f / (3.14159f / 180.0f);

Pose::Quaternion axisAngleToQuaternion(float x, float y, float z, float rotation);

Pose::Quaternion multiplyQuaternions(const Pose::Quaternion& q1, const Pose::Quaternion q2);
}
