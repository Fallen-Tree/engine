#include "math_types.hpp"
#include "glm/common.hpp"

Plane plane_from_triangle(Triangle triangle) {
    Plane plane;
    plane.normal = triangle.normal;
    plane.d = -glm::dot(plane.normal, triangle.verts[0]);
    return plane;
}

Vec3 aabb_closest_point(AABB aabb, Vec3 point) {
    Vec3 result = point;
    result.x = glm::clamp(point.x, aabb.min.x, aabb.max.x);
    result.y = glm::clamp(point.y, aabb.min.y, aabb.max.y);
    result.z = glm::clamp(point.z, aabb.min.z, aabb.max.z);
    return result;
}

bool aabb_plane_test(AABB aabb, Plane plane) {
    Vec3 center = (aabb.min + aabb.max) / 2.0f;
    Vec3 extents = (aabb.max - aabb.min) / 2.0f;

    float r = extents.x * glm::abs(plane.normal.x) +
        extents.y * glm::abs(plane.normal.y) +
        extents.z * glm::abs(plane.normal.z);

    float c_dist = glm::dot(center, plane.normal) - plane.d;
    return glm::abs(c_dist) <= r;
}

bool aabb_aabb_test(AABB lhs, AABB rhs) {
    return lhs.max.x >= rhs.min.x && rhs.max.x >= lhs.min.x &&
        lhs.max.y >= rhs.min.y && rhs.max.y >= lhs.min.y &&
        lhs.max.z >= rhs.min.z && rhs.max.z >= lhs.min.z;
}

bool aabb_triangle_test(AABB aabb, Triangle tri) {
    Vec3 center = (aabb.min + aabb.max) / 2.0f;
    Vec3 length = (aabb.max - aabb.min) / 2.0f;

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

    return aabb_plane_test(aabb, plane_from_triangle(tri));
}
