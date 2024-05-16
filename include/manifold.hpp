#pragma once
#include <limits>
#include "math_types.hpp"

struct CollisionManifold {
    bool collide = false;
    Vec3 collisionNormal = Vec3(0);
    Vec3 collisionPoint;
    float penetrationDistance = std::numeric_limits<float>::max();
};
