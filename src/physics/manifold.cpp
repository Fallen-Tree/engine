#include "manifold.hpp"

void ResetCollisionManifold(CollisionManifold* result) {
    result->isCollide = false;
    result->penetrationDistance = 0;
    result->normal = Vec3(0);
}
