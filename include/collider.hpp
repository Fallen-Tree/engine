#pragma once
#include <variant>
#include <optional>
#include "geometry_primitives.hpp"
#include "transform.hpp"
#include "mesh.hpp"

struct Collider {
    std::variant<AABB, Sphere, OBB, Model *> shape;

    static AABB GetDefaultAABB(Mesh*);
    bool Collide(Transform self, Collider *other, Transform otherTransform);
    bool Raycast(Transform self, Ray ray);
    std::optional<float> RaycastHit(Transform self, Ray ray);
};
