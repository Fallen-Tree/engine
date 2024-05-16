#pragma once
#include <variant>
#include <optional>
#include "geometry_primitives.hpp"
#include "transform.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "manifold.hpp"

struct Collider {
    std::variant<AABB, Sphere, OBB, Mesh *> shape;

    static AABB GetDefaultAABB(Model*);
    static AABB GetDefaultAABB(Mesh*);
    CollisionManifold Collide(Transform self, Collider *other, Transform otherTransform);
    bool Raycast(Transform self, Ray ray);
    std::optional<float> RaycastHit(Transform self, Ray ray);
};
