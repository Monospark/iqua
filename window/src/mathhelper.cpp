#include "mathhelper.h"

#include <cmath>

Pose::Quaternion MathHelper::axisAngleToQuaternion(float x, float y, float z, float rotation)
{
    return Pose::Quaternion{ 
        x * sin(rotation * DEGREES_TO_RADIANS / 2),
        y * sin(rotation * DEGREES_TO_RADIANS / 2),
        z * sin(rotation * DEGREES_TO_RADIANS / 2),
        cos(rotation * DEGREES_TO_RADIANS / 2)
    };
}

Pose::Quaternion MathHelper::multiplyQuaternions(const Pose::Quaternion& q1, const Pose::Quaternion q2)
{
    return Pose::Quaternion{
        q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
        q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
        q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
        q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
    };
}
