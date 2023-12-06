#include "collider.hpp"
#include "collisions.hpp"

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
