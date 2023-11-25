#pragma once
#include "math_types.hpp"
#include "transform.hpp"
#include "model.hpp"

class Collider {
 public:
    virtual bool Collide(Transform self, Collider *other, Transform otherTransform) = 0;
};

class BoxCollider : public Collider {
 public:
    AABB box;
    explicit BoxCollider(AABB);

    bool Collide(Transform self, Collider *other, Transform otherTransform);
};

class SphereCollider : public Collider {
 public:
    Sphere sphere;
    explicit SphereCollider(Sphere);

    bool Collide(Transform self, Collider *other, Transform otherTransform);
};

class MeshCollider : public Collider {
 public:
    Model *model;
    explicit MeshCollider(Model *);

    bool Collide(Transform self, Collider *other, Transform otherTransform);
};
