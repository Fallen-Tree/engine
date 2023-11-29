#pragma once
#include <variant>
#include "transform.hpp"
#include "geometry_primitives.hpp"
#include "model.hpp"

class Collider {
 public:
    std::variant<AABB, Sphere, Model *> shape;

    bool Collide(Transform self, Collider *other, Transform otherTransform);
};
