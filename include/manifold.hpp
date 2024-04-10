#pragma once

#include "math_types.hpp"

struct CollisionManifold {
    bool isCollide = false;
    Vec3 normal = Vec3(0);
    Vec3 collisionPoint;
    float penetrationDistance = 0;
};

inline void ResetCollisionManifold(CollisionManifold* result) {
    result->isCollide = false;
    result->penetrationDistance = 0;
    result->normal = Vec3(0);
}
