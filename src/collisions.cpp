#include "math_types.hpp"
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>

Plane::Plane(Triangle triangle) {
    normal = triangle.normal;
    d = -glm::dot(normal, triangle.verts[0]);
}

Vec3 AABB::ClosestPoint(Vec3 point) {
    Vec3 result = {
        glm::clamp(point.x, min.x, max.x),
        glm::clamp(point.y, min.y, max.y),
        glm::clamp(point.z, min.z, max.z),
    };
    return result;
}

bool AABB::CollidePlane(Plane plane) {
    Vec3 center = (min + max) / 2.0f;
    Vec3 extents = (max - min) / 2.0f;

    float r = extents.x * glm::abs(plane.normal.x) +
        extents.y * glm::abs(plane.normal.y) +
        extents.z * glm::abs(plane.normal.z);

    float c_dist = glm::dot(center, plane.normal) - plane.d;
    return glm::abs(c_dist) <= r;
}

bool AABB::CollideAABB(AABB rhs) {
    return max.x >= rhs.min.x && rhs.max.x >= min.x &&
        max.y >= rhs.min.y && rhs.max.y >= min.y &&
        max.z >= rhs.min.z && rhs.max.z >= min.z;
}

bool AABB::CollideTriangle(Triangle tri) {
    Vec3 center = (min + max) / 2.0f;
    Vec3 length = (max - min) / 2.0f;

    Vec3 verts[3] = {
        tri.verts[0] - center,
        tri.verts[1] - center,
        tri.verts[2] - center,
    };

    Vec3 edges[3] = {
        verts[1] - verts[0],
        verts[2] - verts[1],
        verts[0] - verts[2],
    };

    // a00 - a02
    for (int i = 0; i < 3; i++) {
        float r = length.y * glm::abs(edges[i].z) + length.z * glm::abs(edges[i].y);
        float p1 = -verts[(2 + i) % 3].y * edges[i].z + verts[(2 + i) % 3].z * edges[i].y;
        float p2 = -verts[(3 + i) % 3].y * edges[i].z + verts[(3 + i) % 3].z * edges[i].y;
        if (glm::max(p1, p2) < -r || glm::min(p1, p2) > r) {
            // Separating axis found
            return false;
        }
    }

    // a10 - a12
    for (int i = 0; i < 3; i++) {
        float r = length.x * glm::abs(edges[i].z) + length.z * glm::abs(edges[i].x);
        float p1 = -verts[(2 + i) % 3].x * edges[i].z + verts[(2 + i) % 3].z * edges[i].x;
        float p2 = -verts[(3 + i) % 3].x * edges[i].z + verts[(3 + i) % 3].z * edges[i].x;
        if (glm::max(p1, p2) < -r || glm::min(p1, p2) > r) {
            // Separating axis found
            return false;
        }
    }

    // a20 - a22
    for (int i = 0; i < 3; i++) {
        float r = length.x * glm::abs(edges[i].y) + length.z * glm::abs(edges[i].y);
        float p1 = -verts[(2 + i) % 3].x * edges[i].y + verts[(2 + i) % 3].y * edges[i].x;
        float p2 = -verts[(3 + i) % 3].x * edges[i].y + verts[(3 + i) % 3].y * edges[i].x;
        if (glm::max(p1, p2) < -r || glm::min(p1, p2) > r) {
            // Separating axis found
            return false;
        }
    }

    // testing triangle's AABB with given AABB
    if (glm::max(glm::max(verts[0].x, verts[1].x), verts[2].x) < -length.x ||
            glm::min(glm::min(verts[0].x, verts[1].x), verts[2].x) > length.x) {
        return false;
    }
    if (glm::max(glm::max(verts[0].y, verts[1].y), verts[2].y) < -length.y ||
            glm::min(glm::min(verts[0].y, verts[1].y), verts[2].y) > length.y) {
        return false;
    }
    if (glm::max(glm::max(verts[0].z, verts[1].z), verts[2].z) < -length.z ||
            glm::min(glm::min(verts[0].z, verts[1].z), verts[2].z) > length.z) {
        return false;
    }

    return CollidePlane(Plane(tri));
}

bool Sphere::CollideSphere(Sphere other) {
    return glm::length2(other.center - center) <= (radius + other.radius) * (radius + other.radius);
}
