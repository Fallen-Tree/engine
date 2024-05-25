#pragma once
#include <variant>
#include <optional>
#include "geometry_primitives.hpp"
#include "transform.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "manifold.hpp"


struct Collider {
    std::variant<AABB, Sphere, OBB, Mesh *> shape;

    enum Layer {
        Layer1 = 1,
        Layer2 = 1 << 1,
        Layer3 = 1 << 2,
        Layer4 = 1 << 3,
        Layer5 = 1 << 4,
        Layer6 = 1 << 5,
        Layer7 = 1 << 6,
        Layer8 = 1 << 7,
    };
    int layer = Layer1;

    static AABB GetDefaultAABB(Model*);
    static AABB GetDefaultAABB(Mesh*);

    CollisionManifold Collide(Transform self, Collider *other, Transform otherTransform);
    bool Raycast(Transform self, Ray ray);
    std::optional<float> RaycastHit(Transform self, Ray ray);
};
