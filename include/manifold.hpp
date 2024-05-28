#pragma once
#include <limits>
#include "math_types.hpp"

struct CollisionManifold {
    bool collide = false;
    int pointCnt = 0;
    Vec3 normals[4];
    Vec3 points[4];
    float penetrationDistance = std::numeric_limits<float>::max();
};
