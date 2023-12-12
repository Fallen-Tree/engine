#include <assert.h>
#include "collider.hpp"
#include "collisions.hpp"
#include "logger.hpp"

template<typename U>
bool CollideShifted(Ray lhs, U rhs, Transform rhsTransform) {
    return CollidePrimitive(lhs, rhs.Transformed(rhsTransform));
}

template<>
bool CollideShifted(Ray lhs, Model * rhs, Transform rhsTransform) {
    Logger::Error("Raycast into mesh is not supported yet");
    assert(false);
    return false;
}

template<typename T, typename U>
bool CollideShifted(T lhs, Transform lhsTransform, U rhs, Transform rhsTransform) {
    return CollidePrimitive(lhs.Transformed(lhsTransform), rhs.Transformed(rhsTransform));
}

template<typename T>
bool CollideShifted(T lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform) {
    return CollideModelAt(lhs.Transformed(lhsTransform), rhs, rhsTransform);
}

template<typename U>
bool CollideShifted(Model *lhs, Transform lhsTransform, U rhs, Transform rhsTransform) {
    return CollideModelAt(rhs.Transformed(rhsTransform), lhs, lhsTransform);
}

bool CollideShifted(Model *lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform) {
    return CollideModels(lhs, lhsTransform, rhs, rhsTransform);
}
bool Collider::Collide(Transform self, Collider *other, Transform otherTransform) {
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
std::optional<float> CollisionShifted(Ray lhs, Model * rhs, Transform rhsTransform) {
    Logger::Error("Raycast into mesh is not supported yet");
    assert(false);
    return {};
}
std::optional<float> Collider::RaycastHit(Transform self, Ray ray) {
    return std::visit([=](auto shape) { return CollisionShifted(ray, shape, self); }, shape);
}
