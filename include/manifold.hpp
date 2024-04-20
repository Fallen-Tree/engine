#pragma once
#include <limits>
#include "math_types.hpp"

struct CollisionManifold {
    bool collide = false;
    Vec3 collisionNormal = Vec3(0);
    Vec3 collisionPoint;
    float penetrationDistance = std::numeric_limits<float>::max();
};

inline void ResetCollisionManifold(CollisionManifold* result) {
    result->collide = false;
    result->penetrationDistance = 0;
    result->collisionNormal = Vec3(0);
}
