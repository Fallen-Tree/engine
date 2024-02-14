#include "math_types.hpp"

Vec3 Mul(Vec3 vec, Mat4 mat) {
    Vec4 res = Vec4(vec, 1.0) * mat;
    return Vec3(res.x / res.w, res.y / res.w, res.z / res.w);
}
