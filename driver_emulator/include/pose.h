#pragma once

struct Pose {
    struct Position {
        float x, y, z;
    } position;

    struct Quaternion {
        float x, y, z, w;
    } rotation;
};