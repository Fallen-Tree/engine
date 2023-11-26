#pragma once
#include <variant>
#include "math_types.hpp"
#include "transform.hpp"
#include "model.hpp"

class Collider {
 public:
    using InnerCollider = std::variant<AABB, Sphere, Model *>;
    InnerCollider shape;
    bool Collide(Transform self, Collider *other, Transform otherTransform);

    bool Collide(AABB lhs, Transform lhsTransform, AABB rhs, Transform rhsTransform);

    bool Collide(Model *lhs, Transform lhsTransform, AABB rhs, Transform rhsTransform);
    bool Collide(AABB lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform);

    bool Collide(Sphere lhs, Transform lhsTransform, AABB rhs, Transform rhsTransform);
    bool Collide(AABB lhs, Transform lhsTransform, Sphere rhs, Transform rhsTransform);

    bool Collide(Sphere lhs, Transform lhsTransform, Sphere rhs, Transform rhsTransform);

    bool Collide(Model *lhs, Transform lhsTransform, Sphere rhs, Transform rhsTransform);
    bool Collide(Sphere lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform);

    bool Collide(Model *lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform);
};
