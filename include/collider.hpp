#pragma once
#include <variant>
#include <optional>
#include "geometry_primitives.hpp"
#include "transform.hpp"
#include "mesh.hpp"

struct Collider {
    std::variant<AABB, Sphere, Mesh *> shape;

    static AABB GetDefaultAABB(Model*);
    bool Collide(Transform self, Collider *other, Transform otherTransform);
    bool Raycast(Transform self, Ray ray);
    std::optional<float> RaycastHit(Transform self, Ray ray);
};
