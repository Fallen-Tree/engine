#pragma once
#include "transform.hpp"
#include "math_types.hpp"

struct Plane;
struct Triangle;
struct Sphere;
struct Ray;
struct AABB;

struct AABB {
    Vec3 min, max;

    Vec3 ClosestPoint(Vec3);
    float Distance2(Vec3);
    AABB PrevState(Vec3, float);

    AABB Transformed(Transform);
};

struct Sphere {
    Vec3 center;
    float radius;

    Vec3 ClosestPoint(Vec3);

    Sphere PrevState(Vec3, float);
    Sphere Transformed(Transform);
};

struct Triangle {
    Vec3 a, b, c;
    Vec3 normal;

    explicit Triangle(Vec3, Vec3, Vec3);

    Vec3 ClosestPoint(Vec3);
    float Distance2(Vec3);
    Triangle Transformed(Transform);
};

struct Ray {
    Vec3 direction;
    Vec3 origin;

    Ray(Vec3 from, Vec3 to);
};

struct Plane {
    Vec3 normal;
    float d;

    Plane() = default;
    explicit Plane(Triangle t);
    explicit Plane(Vec3 normal, Vec3 point);
};
