#include "geometry_primitives.hpp"
#include "logger.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>

Plane::Plane(Triangle triangle) {
    *this = Plane(triangle.normal, triangle.a);
}

Plane::Plane(Vec3 normal, Vec3 point) {
    this->normal = normal;
    d = -glm::dot(normal, point);
}

Plane::Plane(Vec3 normal, float distance) {
    this->normal = normal;
    this->d = distance;
}

std::vector<Plane> AABB::GetPlanes() {
    std::vector<Plane> result;
    result.resize(6);
    Vec3 center = (max + min) * 0.5f;

    result[0] = Plane(
            Vec3(1, 0, 0), glm::dot(Vec3(1, 0, 0), Vec3(max.x, center.y, center.z)));
    result[1] = Plane(
            Vec3(-1, 0, 0), -glm::dot(Vec3(1, 0, 0), Vec3(min.x, center.y, center.z)));
    result[2] = Plane(
            Vec3(0, 1, 0), glm::dot(Vec3(0, 1, 0), Vec3(center.x, max.y, center.z)));
    result[3] = Plane(
            Vec3(0, -1, 0), -glm::dot(Vec3(0, 1, 0), Vec3(center.x, min.y, center.z)));
    result[4] = Plane(
            Vec3(0, 0, 1), glm::dot(Vec3(0, 0, 1), Vec3(center.x, center.y, max.z)));
    result[5] = Plane(
            Vec3(0, 0, -1), -glm::dot(Vec3(0, 0, -1), Vec3(center.x, center.y, min.z)));

    return result;
}

bool AABB::IsPointIn(Vec3 point) {
    if (point.x < min.x
            || point.y < min.y
            || point.z < min.z
            || point.x > max.x
            || point.y>max.y
            || point.z>max.z) {
        return false;
    }
    return true;
}

std::vector<Vec3> AABB::GetVertices() {
    std::vector<Vec3> v;
    v.resize(8);

    v[0] = Vec3(max.x, max.y, max.z);
    v[1] = Vec3(min.x, max.y, max.z);
    v[2] = Vec3(max.x, min.y, max.z);
    v[3] = Vec3(max.x, max.y, min.z);
    v[4] = Vec3(min.x, min.y, max.z);
    v[5] = Vec3(max.x, min.y, min.z);
    v[6] = Vec3(min.x, max.y, min.z);
    v[7] = Vec3(min.x, min.y, min.z);

    return v;
}

std::vector<Line> AABB::GetEdges() {
    std::vector<Line> result;
    result.reserve(12);
    std::vector<Vec3> v = GetVertices();

    int index[][2] = {  // Indices of edge-vertices
        {6, 1}, {6, 3}, {6, 4}, {2, 7}, {2, 5}, {2, 0},
        {0, 1}, {0, 3}, {7, 1}, {7, 4}, {4, 5}, {5, 3}
    };
    for (int j = 0; j < 12; ++j) {
        result.push_back(Line{
                v[index[j][0]],
                v[index[j][1]]});
    }

    return result;
}

Vec3 AABB::ClosestPoint(Vec3 point) {
    Vec3 result = {
        glm::clamp(point.x, min.x, max.x),
        glm::clamp(point.y, min.y, max.y),
        glm::clamp(point.z, min.z, max.z),
    };
    return result;
}

Interval AABB::GetInterval(Vec3 axis) {
    Vec3 i = min;
    Vec3 a = max;
    Vec3 vertex[8] = {
        Vec3(i.x, a.y, a.z),
        Vec3(i.x, a.y, i.z),
        Vec3(i.x, i.y, a.z),
        Vec3(i.x, i.y, i.z),
        Vec3(a.x, a.y, a.z),
        Vec3(a.x, a.y, i.z),
        Vec3(a.x, i.y, a.z),
        Vec3(a.x, i.y, i.z)
    };

    Interval result;
    result.min = result.max = glm::dot(axis, vertex[0]);
    for (int i = 1; i < 8; ++i) {
        float projection = glm::dot(axis, vertex[i]);
        result.min = (projection < result.min) ?
            projection : result.min;
        result.max = (projection > result.max) ?
            projection : result.max;
    }

    return result;
}


Vec3 Sphere::ClosestPoint(Vec3 point) {
    return center + radius
        * Norm(point - center);
}

float AABB::Distance2(Vec3 point) {
    float res = 0.0f;
    if (point.x < min.x) {
        res += (point.x - min.x) * (point.x - min.x);
    } else if (point.x > max.x) {
        res += (point.x - max.x) * (point.x - max.x);
    }

    if (point.y < min.y) {
        res += (point.y - min.y) * (point.y - min.y);
    } else if (point.y > max.y) {
        res += (point.y - max.y) * (point.y - max.y);
    }

    if (point.z < min.z) {
        res += (point.z - min.z) * (point.z - min.z);
    } else if (point.z > max.z) {
        res += (point.z - max.z) * (point.z - max.z);
    }

    return res;
}

AABB AABB::Transformed(Transform transform) {
    return AABB {
        min * transform.GetScale() + transform.GetTranslation(),
        max * transform.GetScale() + transform.GetTranslation()
    };
}

AABB AABB::PrevState(Vec3 velocity, float dt) {
    return AABB {
        min - velocity * dt,
        max - velocity * dt,
    };
}

Triangle::Triangle(Vec3 a, Vec3 b, Vec3 c) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->normal = glm::cross(a - b, a - c);
}

// This function can be made faster pretty easy
// If it'll prove to be slow - I will rewrite
// TODO(theblek): rewrite this with dot products. See the book.
Vec3 Triangle::ClosestPoint(Vec3 point) {
    Vec3 ab = b - a;
    Vec3 ac = c - a;
    Vec3 bc = c - b;

    float snom = glm::dot(point - a, ab);
    float tnom = glm::dot(point - a, ac);
    if (snom <= 0.0f && tnom <= 0.0f) {
        return a;
    }

    float sdenom = glm::dot(point - b, a - b);
    float unom = glm::dot(point - b, bc);
    if (sdenom <= 0.0f && unom <= 0.0f) {
        return b;
    }

    float udenom = glm::dot(point - c, b - c);
    float tdenom = glm::dot(point - c, a - c);
    if (udenom <= 0.0f && tdenom <= 0.0f) {
        return c;
    }

    float va = glm::dot(normal, glm::cross(a - point, b - point));
    if (va <= 0.0f && snom >= 0.0f && sdenom >= 0.0f) {
        return a + snom / (snom + sdenom) * ab;
    }

    float vb = glm::dot(normal, glm::cross(b - point, c - point));
    if (vb <= 0.0f && unom >= 0.0f && udenom >= 0.0f) {
        return a + unom / (unom + udenom) * bc;
    }

    float vc = glm::dot(normal, glm::cross(c - point, a - point));
    if (vc <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f) {
        return a + tnom / (tnom + tdenom) * ac;
    }

    // Inside
    float u = va / (va + vb + vc);
    float v = vb / (va + vb + vc);
    float w = 1 - u - v;

    return u*a + v*b + w*c;
}

Interval Triangle::GetInterval(Vec3 axis) {
    Interval result;

    result.min = glm::dot(axis, a);
    result.max = result.min;

    float value = glm::dot(axis, b);
    result.min = fminf(result.min, value);
    result.max = fmaxf(result.max, value);

    value = glm::dot(axis, c);
    result.min = fminf(result.min, value);
    result.max = fmaxf(result.max, value);

    return result;
}

float Triangle::Distance2(Vec3 point) {
    return glm::length2(point - ClosestPoint(point));
}

Triangle Triangle::Transformed(Transform transform) {
    auto mat = glm::transpose(transform.GetTransformMatrix());
    auto mul = [](Vec3 v, Mat4 mat) {
        Vec4 res = Vec4(v, 1) * mat;
        return Vec3(res.x / res.w, res.y / res.w, res.z / res.w);
    };
    return Triangle(mul(a, mat), mul(b, mat), mul(c, mat));
}

Sphere Sphere::Transformed(Transform transform) {
    auto scale = transform.GetScale();
    if (scale.x != scale.y || scale.y != scale.z) {
        Logger::Error(
         "GEOMETRY_PRIMITIVES::SPHERE::TRANSFORMED::SCALE_SHOULD_BE_EQUAL_ON_ALL_AXES");
    }
    return Sphere{center + transform.GetTranslation(), radius * scale.x};
}

Sphere Sphere::PrevState(Vec3 velocity, float dt) {
    return Sphere {
        center - velocity * dt,
        radius,
    };
}

Ray::Ray(Vec3 from, Vec3 to) {
    origin = from;
    direction = glm::normalize(to - from);
}

OBB OBB::Transformed(Transform transform) {
    return OBB {
        center + transform.GetTranslation(),
        Mat3(transform.GetRotation()) * axis,
        halfWidth * transform.GetScale(),
    };
}

bool OBB::IsPointIn(Vec3 point) {
    Vec3 dir = point - center;
    for (int i = 0; i < 3; i++) {
        float distance = glm::dot(dir, axis[i]);
        if (distance > halfWidth[i] || distance < -halfWidth[i]) {
            return false;
        }
    }

    return true;
}

std::vector<Vec3> OBB::GetVertices() {
    std::vector<Vec3> v;
    v.resize(8);

    v[0] = center + axis[0] * halfWidth[0] + axis[1] * halfWidth[1]
        + axis[2] * halfWidth[2];
    v[1] = center - axis[0] * halfWidth[0] + axis[1] * halfWidth[1]
        + axis[2] * halfWidth[2];
    v[2] = center + axis[0] * halfWidth[0] - axis[1] * halfWidth[1]
        + axis[2] * halfWidth[2];
    v[3] = center + axis[0] * halfWidth[0] + axis[1] * halfWidth[1]
        - axis[2] * halfWidth[2];
    v[4] = center - axis[0] * halfWidth[0] - axis[1] * halfWidth[1]
        - axis[2] * halfWidth[2];
    v[5] = center + axis[0] * halfWidth[0] - axis[1] * halfWidth[1]
        - axis[2] * halfWidth[2];
    v[6] = center - axis[0] * halfWidth[0] + axis[1] * halfWidth[1]
        - axis[2] * halfWidth[2];
    v[7] = center - axis[0] * halfWidth[0] - axis[1] * halfWidth[1]
        + axis[2] * halfWidth[2];

    return v;
}

std::vector<Line> OBB::GetEdges() {
    std::vector<Line> result;
    result.reserve(12);
    std::vector<Vec3> v = GetVertices();

    int index[][2] = {  // Indices of edge-vertices
        {6, 1}, {6, 3}, {6, 4}, {2, 7}, {2, 5}, {2, 0},
        {0, 1}, {0, 3}, {7, 1}, {7, 4}, {4, 5}, {5, 3}
    };
    for (int j = 0; j < 12; ++j) {
        result.push_back(Line{
                v[index[j][0]],
                v[index[j][1]]});
    }

    return result;
}

std::vector<Plane> OBB::GetPlanes() {
    std::vector<Plane> result;
    result.resize(6);

    result[0] = Plane(
            axis[0], glm::dot(axis[0], (center + axis[0] * halfWidth.x)));
    result[1] = Plane(
            axis[0] * (-1.0f), -glm::dot(axis[0], (center - axis[0] * halfWidth.x)));
    result[2] = Plane(
            axis[1], glm::dot(axis[1], (center + axis[1] * halfWidth.y)));
    result[3] = Plane(
            axis[1] * (-1.0f), -glm::dot(axis[1], (center - axis[1] * halfWidth.y)));
    result[4] = Plane(
            axis[2], glm::dot(axis[2], (center + axis[2] * halfWidth.z)));
    result[5] = Plane(
        axis[2] * (-1.0f), -glm::dot(axis[2], (center - axis[2] * halfWidth.z)));

    return result;
}

Interval OBB::GetInterval(Vec3 axisPar) {
    Vec3 vertex[8];

    vertex[0] = center + axis[0] * halfWidth[0]
        + axis[1] * halfWidth[1] + axis[2] * halfWidth[2];
    vertex[1] = center - axis[0] * halfWidth[0]
        + axis[1] * halfWidth[1] + axis[2] * halfWidth[2];
    vertex[2] = center + axis[0] * halfWidth[0]
        - axis[1] * halfWidth[1] + axis[2] * halfWidth[2];
    vertex[3] = center + axis[0] * halfWidth[0]
        + axis[1] * halfWidth[1] - axis[2] * halfWidth[2];
    vertex[4] = center - axis[0] * halfWidth[0]
        - axis[1] * halfWidth[1] - axis[2] * halfWidth[2];
    vertex[5] = center + axis[0] * halfWidth[0]
        - axis[1] * halfWidth[1] - axis[2] * halfWidth[2];
    vertex[6] = center - axis[0] * halfWidth[0]
        + axis[1] * halfWidth[1] - axis[2] * halfWidth[2];
    vertex[7] = center - axis[0] * halfWidth[0]
        - axis[1] * halfWidth[1] + axis[2] * halfWidth[2];

    Interval result;
    result.min = result.max = glm::dot(axisPar, vertex[0]);
    for (int i = 1; i < 8; i++) {
        float projection = glm::dot(axisPar, vertex[i]);
        result.min = (projection < result.min) ?
            projection : result.min;
        result.max = (projection > result.max) ?
            projection : result.max;
    }

    return result;
}

Vec3 OBB::ClosestPoint(Vec3 point) {
    const Vec3 d = point - center;

    Vec3 res = center;
    // for each OBB axis
    for (int i = 0; i < 3; i++) {
        float dist = glm::dot(d, axis[i]);

        const float halfwidth = halfWidth[i];
        if (dist > halfwidth) dist = halfwidth;
        if (dist < -halfwidth) dist = -halfwidth;

        res += dist * axis[i];
    }

    return res;
}

float OBB::Distance2(Vec3 point) {
    return glm::dot(ClosestPoint(point) - point, ClosestPoint(point) - point);
}
