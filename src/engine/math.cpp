#include "math_types.hpp"

Vec3 Mul(Vec3 vec, Mat4 mat) {
    Vec4 res = Vec4(vec, 1.0) * mat;
    return Vec3(res.x / res.w, res.y / res.w, res.z / res.w);
}

Vec3 Norm(Vec3 vec) {
    const float epsilon = 1e-9;
    if (glm::length(vec) < epsilon) {
        return Vec3(0);
    }
    return glm::normalize(vec);
}

Vec3 Projection(Vec3 vec, Vec3 axis) {
    auto normAxis = Norm(axis);
    return glm::dot(normAxis, vec) * normAxis;
}
