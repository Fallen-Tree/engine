#include "math_types.hpp"

Vec3 Mul(Vec3 vec, Mat4 mat) {
    Vec4 res = Vec4(vec, 1.0) * mat;
    return Vec3(res.x / res.w, res.y / res.w, res.z / res.w);
}

Vec3 Norm(Vec3 vec) {
    if (glm::length(vec) == 0) {
        return vec;
    }
    return glm::normalize(vec);
}

Vec3 Projection(Vec3 vec, Vec3 axes) {
    auto normAxes = Norm(axes);
    return glm::dot(normAxes, vec) * normAxes;
}
