#pragma once
#include <glm/glm.hpp>

typedef glm::fvec2 Vec2;
typedef glm::fvec3 Vec3;
typedef glm::fvec4 Vec4;

typedef glm::fmat3 Mat3;
typedef glm::fmat4 Mat4;

struct Plane;
struct Triangle;
struct Sphere;
struct Ray;
struct AABB;

struct AABB {
    Vec3 min, max;

    Vec3 ClosestPoint(Vec3);
    float Distance2(Vec3);
};

struct Sphere {
    Vec3 center;
    float radius;
};

struct Triangle {
    Vec3 a, b, c;
    Vec3 normal;

    explicit Triangle(Vec3, Vec3, Vec3);

    Vec3 ClosestPoint(Vec3);
    float Distance2(Vec3);
};

struct Ray {
    Vec3 direction;
    Vec3 origin;
};

struct Plane {
    Vec3 normal;
    float d;

    Plane() = default;
    explicit Plane(Triangle t);
    explicit Plane(Vec3 normal, Vec3 point);
};
