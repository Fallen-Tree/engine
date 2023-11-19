#pragma once
#include <glm/glm.hpp>

typedef glm::fvec2 Vec2;
typedef glm::fvec3 Vec3;
typedef glm::fvec4 Vec4;

typedef glm::fmat3 Mat3;
typedef glm::fmat4 Mat4;

struct AABB {
    Vec3 min, max;
};

struct Plane {
    Vec3 normal;
    float d;
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
