#include "logger.hpp"
#include "math_types.hpp"
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>

Plane::Plane(Triangle triangle) {
    normal = triangle.normal;
    d = -glm::dot(normal, triangle.a);
}

Vec3 AABB::ClosestPoint(Vec3 point) {
    Vec3 result = {
        glm::clamp(point.x, min.x, max.x),
        glm::clamp(point.y, min.y, max.y),
        glm::clamp(point.z, min.z, max.z),
    };
    return result;
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
        tri.a - center,
        tri.b - center,
        tri.c - center,
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

bool Sphere::CollideAABB(AABB aabb) {
    return aabb.Distance2(center) <= radius * radius;
}

bool Sphere::CollideTriangle(Triangle t) {
    return t.Distance2(center) <= radius * radius;
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

float Triangle::Distance2(Vec3 point) {
    return glm::length2(point - ClosestPoint(point));
}

bool Triangle::CollideTriangle(Triangle triangle) {
    Plane aPlane = Plane(*this);
    float sdistb[3] = {
        glm::dot(aPlane.normal, triangle.a) + aPlane.d,
        glm::dot(aPlane.normal, triangle.b) + aPlane.d,
        glm::dot(aPlane.normal, triangle.c) + aPlane.d,
    };
    
    if (glm::abs(sdistb[0]) == 0 && glm::abs(sdistb[1]) == 0 && glm::abs(sdistb[2]) == 0) {
        Logger::Error("Coplanar case for triangle-triangle collision is not handled");
        return false;
    }

    if (sdistb[0] * sdistb[1] > 0 && sdistb[1] * sdistb[2] > 0) {
        return false;
    }
    Plane bPlane = Plane(*this);
    float sdista[3] = {
        glm::dot(aPlane.normal, triangle.a) + aPlane.d,
        glm::dot(aPlane.normal, triangle.b) + aPlane.d,
        glm::dot(aPlane.normal, triangle.c) + aPlane.d,
    };

    if (glm::abs(sdista[0]) == 0 && glm::abs(sdista[1]) == 0 && glm::abs(sdista[2]) == 0) {
        Logger::Error("Coplanar case for triangle-triangle collision is not handled");
        return false;
    }

    if (sdista[0] * sdista[1] > 0 && sdista[1] * sdista[2] > 0) {
        return false;
    }

    Vec3 intersectDir = glm::cross(aPlane.normal, bPlane.normal);
    int projectionIndex = 2;
    float maxAxis = glm::max(glm::max(intersectDir[0], intersectDir[1]), intersectDir[2]);
    if (intersectDir[0] == maxAxis) {
        projectionIndex = 0;
    } else if (intersectDir[1] == maxAxis) {
        projectionIndex = 1;
    }

    auto findInterval = [=](float sdist[3], Vec3 points[3]) -> std::pair<float, float> {
        int otherSideVertex = 1;
        int oneSideVertices[2] = {0, 2};
        if (sdist[0] * sdist[1] > 0) {
            otherSideVertex = 2;
            oneSideVertices[1] = 1;
        } else if (sdist[1] * sdist[2] > 0) {
            otherSideVertex = 0;
            oneSideVertices[0] = 1;
        }

        float proj[3] = {
            points[0][projectionIndex],
            points[1][projectionIndex],
            points[2][projectionIndex],
        };
        int v = oneSideVertices[0];
        float t1 = proj[v] +
            (proj[otherSideVertex] - proj[v]) * sdist[v] / (sdist[v] - sdist[otherSideVertex]);
        v = oneSideVertices[1];
        float t2 = proj[v] +
            (proj[otherSideVertex] - proj[v]) * sdist[v] / (sdist[v] - sdist[otherSideVertex]);
        return std::make_pair(t1, t2);
    };

    // TODO(theblek): fix that. Maybe go back to array in triangle
    // VERY FUCKING BAD                  v
    auto [t1, t2] = findInterval(sdista, &a);
    auto [t3, t4] = findInterval(sdistb, &triangle.a);
    return (glm::max(t1, t2) > t3 && t3 > glm::min(t1, t2))
        || (glm::max(t1, t2) > t4 && t4 > glm::min(t1, t2));
}
