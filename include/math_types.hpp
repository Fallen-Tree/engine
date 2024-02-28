#pragma once
#include <glm/glm.hpp>

typedef glm::fvec2 Vec2;
typedef glm::ivec2 Vec2Int;
typedef glm::fvec3 Vec3;
typedef glm::ivec3 Vec3Int;
typedef glm::fvec4 Vec4;
typedef glm::ivec4 Vec4Int;

typedef glm::fmat3 Mat3;
typedef glm::fmat4 Mat4;

Vec3 Mul(Vec3, Mat4);
