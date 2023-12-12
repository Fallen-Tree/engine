#pragma once
#include <variant>
#include <optional>
#include "geometry_primitives.hpp"
#include "transform.hpp"
#include "model.hpp"

struct Collider {
    std::variant<AABB, Sphere, Model *> shape;

    bool Collide(Transform self, Collider *other, Transform otherTransform);
    bool Raycast(Transform self, Ray ray);
    std::optional<float> RaycastHit(Transform self, Ray ray);
};
