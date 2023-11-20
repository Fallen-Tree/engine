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
    bool PlaneTest(Plane);
    bool AABBTest(AABB);
    bool TriangleTest(Triangle);
};

struct Sphere {
    Vec3 center;
    float radius;
};

struct Triangle {
    Vec3 verts[3];
    Vec3 normal;
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
};
