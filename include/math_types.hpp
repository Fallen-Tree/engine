#pragma once
#include <algorithm>
#include <glm/glm.hpp>

inline bool isCloseToZero(float x) {
    return std::abs(x) <= FLT_EPSILON * std::max(1.f, x);
}

typedef glm::fvec2 Vec2;
typedef glm::ivec2 Vec2Int;
typedef glm::fvec3 Vec3;
typedef glm::ivec3 Vec3Int;
typedef glm::fvec4 Vec4;
typedef glm::ivec4 Vec4Int;

typedef glm::fmat3 Mat3;
typedef glm::fmat4 Mat4;

Vec3 Mul(Vec3, Mat4);
Vec3 Norm(Vec3);
Vec3 Projection(Vec3 vec, Vec3 axis);
