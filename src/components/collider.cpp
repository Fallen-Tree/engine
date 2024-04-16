#include <assert.h>
#include "collider.hpp"
#include "collisions.hpp"
#include "logger.hpp"

AABB Collider::GetDefaultAABB(Mesh* m) {
    int len = m->getLenArrPoints();
    assert(len > 0);
    Vertex* cur = m->getPoints();
    Vec3 min = cur[0].Position;
    Vec3 max = min;

    for (int i = 0; i < len; i ++) {
        min = glm::min(min, cur[i].Position);
        max = glm::max(max, cur[i].Position);
    }
    return AABB{min, max};
}

template<typename U>
bool CollideShifted(Ray lhs, U rhs, Transform rhsTransform) {
    return CollidePrimitive(lhs, rhs.Transformed(rhsTransform));
}

template<>
bool CollideShifted(Ray lhs, Mesh* rhs, Transform rhsTransform) {
    Logger::Error("Raycast into mesh is not supported yet");
    assert(false);
    return false;
}

template<typename T, typename U>
CollisionManifold CollideShifted(T lhs, Transform lhsTransform,
        U rhs, Transform rhsTransform) {
    CollisionManifold res;
    CollidePrimitive(lhs.Transformed(lhsTransform),
            rhs.Transformed(rhsTransform), &res);
    return res;
}

template<typename T>
CollisionManifold CollideShifted(T lhs, Transform lhsTransform,
        Mesh *rhs, Transform rhsTransform) {
    CollisionManifold res;
    CollideMeshAt(lhs.Transformed(lhsTransform), rhs, rhsTransform, &res);
    return res;
}

template<typename U>
CollisionManifold CollideShifted(Mesh *lhs, Transform lhsTransform,
        U rhs, Transform rhsTransform) {
    CollisionManifold res;
    CollideMeshAt(rhs.Transformed(rhsTransform), lhs, lhsTransform, &res);
    return res;
}

CollisionManifold CollideShifted(Mesh *lhs, Transform lhsTransform, Mesh *rhs, Transform rhsTransform) {
    CollisionManifold res;
    CollideMeshes(lhs, lhsTransform, rhs, rhsTransform, &res);
    return res;
}

CollisionManifold Collider::Collide(Transform self, Collider *other, Transform otherTransform) {
    return std::visit([=](auto var1) {
            return std::visit(
                [=](auto var2) {
                    return CollideShifted(var1, self, var2, otherTransform);
                }, other->shape);
        }, shape);
}

bool Collider::Raycast(Transform self, Ray ray) {
    return std::visit([=](auto shape) { return CollideShifted(ray, shape, self); }, shape);
}

template<typename U>
std::optional<float> CollisionShifted(Ray lhs, U rhs, Transform rhsTransform) {
    return CollisionPrimitive(lhs, rhs.Transformed(rhsTransform));
}

template<>
std::optional<float> CollisionShifted(Ray lhs, Mesh * rhs, Transform rhsTransform) {
    Logger::Error("Raycast into mesh is not supported yet");
    assert(false);
    return {};
}
std::optional<float> Collider::RaycastHit(Transform self, Ray ray) {
    return std::visit([=](auto shape) { return CollisionShifted(ray, shape, self); }, shape);
}
