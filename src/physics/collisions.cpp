#include <variant>
#include <optional>
#include "logger.hpp"
#include "math_types.hpp"
#include "collisions.hpp"
#include "engine_config.hpp"
#include "pretty_print.hpp"
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

void CollidePrimitive(OBB obb, AABB aabb, CollisionManifold* manifold) {
    CollidePrimitive(aabb, obb, manifold);
    manifold->normal *= -1;
}

inline float PenetrationDepth(OBB& o, AABB& a,
        const Vec3& axis, bool* outShouldFlip) {
    Interval i1 = o.GetInterval(Norm(axis));
    Interval i2 = a.GetInterval(Norm(axis));

    if (!((i2.min <= i1.max) && (i1.min <= i2.max))) {
        return 0.0f;  // No penerattion
    }

    float len1 = i1.max - i1.min;
    float len2 = i2.max - i2.min;
    float min = fminf(i1.min, i2.min);
    float max = fmaxf(i1.max, i2.max);
    float length = max - min;

    if (outShouldFlip != 0) {
        *outShouldFlip = (i2.min < i1.min);
    }

    return (len1 + len2) - length;
}

void CollidePrimitive(AABB aabb, OBB obb, CollisionManifold* manifold) {
    Vec3 test[15] = {
        Vec3(1, 0, 0),
        // AABB axis 1
        Vec3(0, 1, 0),
        // AABB axis 2
        Vec3(0, 0, 1),
        // AABB axis 3
        obb.axis[0],  // OBB axis 1
        obb.axis[1],  // OBB axis 2
        obb.axis[2]   // OBB axis 3
        // We will fill out the remaining axis in the next step
    };

    for (int i = 0; i < 3; i++) {  // Fill out rest of axis
        test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
        test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
        test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
    }

    Vec3 *hitNormal = nullptr;
    bool shouldFlip;

    for (int i = 0; i < 15; i++) {
        if (glm::length(test[i]) < 0.001f) {
            continue;
        }
        float depth = PenetrationDepth(obb, aabb, test[i], &shouldFlip);
        if (depth <= 0.0f) {
            return;
        } else if (depth < manifold->penetrationDistance) {
            if (shouldFlip) {
                test[i] = test[i] * -1.0f;
            }
            manifold->penetrationDistance = depth;
            hitNormal = &test[i];
        }
    }

    if (hitNormal == nullptr) {
        return;
    }

    Vec3 axis = Norm(*hitNormal);

    // TODO(solloballon): find all collision point
    manifold->collisionPoint = obb.ClosestPoint((aabb.max + aabb.min) / 2.f);

    manifold->isCollide = true;
    manifold->normal = axis;
    return;  // Seperating axis not found
}

bool OverlapOnAxis(OBB obb, Triangle triangle, Vec3 axis) {
    Interval a = obb.GetInterval(axis);
    Interval b = triangle.GetInterval(axis);
    return ((b.min <= a.max) && (a.min <= b.max));
}

void CollidePrimitive(Triangle triangle, OBB obb, CollisionManifold *manifold) {
    // Compute the edge vectors of the triangle (ABC)
    Vec3 f0 = triangle.b - triangle.a;
    Vec3 f1 = triangle.c - triangle.b;
    Vec3 f2 = triangle.a - triangle.c;

    Vec3 test[13] = {
        obb.axis[0],
        obb.axis[1],
        obb.axis[2],
        triangle.normal,
        glm::cross(obb.axis[0], f0),
        glm::cross(obb.axis[0], f1),
        glm::cross(obb.axis[0], f2),
        glm::cross(obb.axis[1], f0),
        glm::cross(obb.axis[1], f1),
        glm::cross(obb.axis[1], f2),
        glm::cross(obb.axis[2], f0),
        glm::cross(obb.axis[2], f1),
        glm::cross(obb.axis[2], f2)
    };

    for (int i = 0; i < 13; i++) {
        if (!OverlapOnAxis(obb, triangle, test[i])) {
            return;  // Separating axis found
        }
    }

    manifold->isCollide = true;
    return;  // Separating axis not found
}

void CollidePrimitive(OBB obb, Triangle triangle, CollisionManifold *manifold) {
    CollidePrimitive(triangle, obb, manifold);
}

// TODO(solloballon): do this shit
bool CollidePrimitive(Ray, OBB) {
    assert(false);
    return false;
}

inline bool ClipToPlane(const Plane& plane,
        const Line& line, Vec3* outPoint) {
    Vec3 ab = line.end - line.start;
    float nAB = glm::dot(plane.normal, ab);
    if (isCloseToZero(nAB)) {
        return false;
    }

    float nA = glm::dot(plane.normal, line.start);
    float t = (plane.d - nA) / nAB;

    if (t >= 0.0f && t <= 1.0f) {
        if (outPoint != 0) {
            *outPoint = line.start + ab * t;
        }
        return true;
    }

    return false;
}

inline std::vector<Vec3> ClipEdgesToOBB(
        const std::vector<Line>& edges, OBB& obb) {
    std::vector<Vec3> result;
    result.reserve(edges.size());
    Vec3 intersection;

    std::vector<Plane> planes = obb.GetPlanes();
    for (int i = 0; i < planes.size(); i++) {
        for (int j = 0; j < edges.size(); j++) {
            if (ClipToPlane(planes[i], edges[j], &intersection)) {
                if (obb.PointIn(intersection)) {
                    result.push_back(intersection);
                }
            }
        }
    }

    return result;
}

inline float PenetrationDepth(OBB& o1, OBB& o2,
        const Vec3& axis, bool* outShouldFlip) {
    Interval i1 = o1.GetInterval(Norm(axis));
    Interval i2 = o2.GetInterval(Norm(axis));

    if (!((i2.min <= i1.max) && (i1.min <= i2.max))) {
        return 0.0f;  // No penerattion
    }

    float len1 = i1.max - i1.min;
    float len2 = i2.max - i2.min;
    float min = fminf(i1.min, i2.min);
    float max = fmaxf(i1.max, i2.max);
    float length = max - min;

    if (outShouldFlip != 0) {
        *outShouldFlip = (i2.min < i1.min);
    }

    return (len1 + len2) - length;
}

void CollidePrimitive(OBB a, OBB b, CollisionManifold *manifold) {
    Vec3 test[15] = {  // Face axis
        a.axis[0],
        a.axis[1],
        a.axis[2],
        b.axis[0],
        b.axis[1],
        b.axis[2],
    };
    for (int i = 0; i < 3; i++) {  // Fill out rest of axis
        test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
        test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
        test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
    }

    Vec3 *hitNormal = nullptr;
    bool shouldFlip;

    for (int i = 0; i < 15; i++) {
        if (glm::length(test[i]) < 0.001f) {
            continue;
        }
        float depth = PenetrationDepth(a, b, test[i], &shouldFlip);
        if (depth <= 0.0f) {
            return;
        } else if (depth < manifold->penetrationDistance) {
            if (shouldFlip) {
                test[i] = test[i] * -1.0f;
            }
            manifold->penetrationDistance = depth;
            hitNormal = &test[i];
        }
    }

    if (hitNormal == nullptr) {
        return;
    }
    Vec3 axis = Norm(*hitNormal);

    std::vector<Vec3> c1 = ClipEdgesToOBB(b.GetEdges(), a);
    std::vector<Vec3> c2 = ClipEdgesToOBB(a.GetEdges(), b);

    Vec3 p = Vec3(0);
    for (auto i : c1) p += i;
    for (auto i : c2) p += i;
    if (c1.size() == 0 && c2.size() == 0)
        Logger::Info("COLLISION::OBBVSOBB::POINTS_SIZE_IS_ZERO");
    manifold->collisionPoint = p / (float)(c1.size() + c2.size());

    Interval i = a.GetInterval(axis);
    float distance = (i.max - i.min) * 0.5f
        - manifold->penetrationDistance * 0.5f;
    Vec3 pointOnPlane = a.center + axis * distance;
    manifold->collisionPoint += (axis *
            glm::dot(axis, pointOnPlane - manifold->collisionPoint));

    manifold->isCollide = true;
    manifold->normal = -axis;

    return;
}

void CollidePrimitive(Sphere a, OBB b, CollisionManifold *manifold) {
    Vec3 p = b.ClosestPoint(a.center);
    float distanceSq = glm::length(p - a.center);
    manifold->isCollide = distanceSq <= a.radius * a.radius;
    if (!manifold->isCollide)
        return;

    if (isCloseToZero(distanceSq)) {
        float mSq = glm::length(p - b.center);
        if (isCloseToZero(mSq))  // here manifold can be strange
            return;
        // Closest point is at the center of the sphere
        manifold->normal = Norm(p - b.center);
    } else {
        manifold->normal = Norm(a.center - p);
    }

    Vec3 outsidePoint = a.center - manifold->normal * a.radius;
    float distance = glm::length(p - outsidePoint);
    manifold->collisionPoint = p + (outsidePoint - p) * 0.5f;
    manifold->penetrationDistance = distance * 0.5f;
}

void CollidePrimitive(OBB a, Sphere b, CollisionManifold *manifold) {
    CollidePrimitive(b, a, manifold);
    manifold->normal *= -1;
}

void CollidePrimitive(Plane a, OBB b, CollisionManifold *manifold) {
    CollidePrimitive(b, a, manifold);
}

void CollidePrimitive(OBB a, Plane b, CollisionManifold *manifold) {
    float r = a.halfWidth[0] + std::abs(glm::dot(b.normal, a.axis[0]))
        + a.halfWidth[1] + std::abs(glm::dot(b.normal, a.axis[1]))
        + a.halfWidth[2] + std::abs(glm::dot(b.normal, a.axis[2]));
    manifold->isCollide = std::abs(glm::dot(b.normal, a.center)- b.d) <= r;
}

void CollidePrimitive(Plane p, AABB a, CollisionManifold *manifold) {
    CollidePrimitive(a, p, manifold);
}

void CollidePrimitive(AABB aabb, Plane plane, CollisionManifold *manifold) {
    Vec3 center = (aabb.min + aabb.max) / 2.0f;
    Vec3 extents = (aabb.max - aabb.min) / 2.0f;

    float r = extents.x * glm::abs(plane.normal.x) +
        extents.y * glm::abs(plane.normal.y) +
        extents.z * glm::abs(plane.normal.z);

    float c_dist = glm::dot(center, plane.normal) + plane.d;
    manifold->isCollide = glm::abs(c_dist) <= r;
}

inline float PenetrationDepth(AABB& a1, AABB& a2,
        const Vec3& axis, bool* outShouldFlip) {
    Interval i1 = a1.GetInterval(axis);
    Interval i2 = a2.GetInterval(axis);

    if (!((i2.min <= i1.max) && (i1.min <= i2.max))) {
        return 0.0f;  // No penerattion
    }

    float len1 = i1.max - i1.min;
    float len2 = i2.max - i2.min;
    float min = fminf(i1.min, i2.min);
    float max = fmaxf(i1.max, i2.max);
    float length = max - min;

    if (outShouldFlip != 0) {
        *outShouldFlip = (i2.min < i1.min);
    }

    return (len1 + len2) - length;
}

void CollidePrimitive(AABB a1, AABB a2, CollisionManifold *manifold) {
    Vec3 test[3] {
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        Vec3(0, 0, 1),
    };

    Vec3 *hitNormal = nullptr;
    bool shouldFlip;

    for (int i = 0; i < 3; i++) {
        float depth = PenetrationDepth(a1, a2, test[i], &shouldFlip);
        if (depth <= 0.0f) {
            return;
        } else if (depth < manifold->penetrationDistance) {
            if (shouldFlip) {
                test[i] = test[i] * -1.0f;
            }
            manifold->penetrationDistance = depth;
            hitNormal = &test[i];
        }
    }

    if (hitNormal == nullptr) {
        return;
    }

    manifold->collisionPoint = Vec3(0);
    manifold->isCollide = true;
    manifold->normal = -(*hitNormal);
}

void CollidePrimitive(Triangle t, AABB a, CollisionManifold *manifold) {
    CollidePrimitive(a, t, manifold);
}

void CollidePrimitive(AABB aabb, Triangle tri, CollisionManifold *manifold) {
    Vec3 center = (aabb.min + aabb.max) / 2.0f;
    Vec3 length = (aabb.max - aabb.min) / 2.0f;

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
            return;
        }
    }

    // a10 - a12
    for (int i = 0; i < 3; i++) {
        float r = length.x * glm::abs(edges[i].z) + length.z * glm::abs(edges[i].x);
        float p1 = -verts[(2 + i) % 3].x * edges[i].z + verts[(2 + i) % 3].z * edges[i].x;
        float p2 = -verts[(3 + i) % 3].x * edges[i].z + verts[(3 + i) % 3].z * edges[i].x;
        if (glm::max(p1, p2) < -r || glm::min(p1, p2) > r) {
            // Separating axis found
            return;
        }
    }

    // a20 - a22
    for (int i = 0; i < 3; i++) {
        float r = length.x * glm::abs(edges[i].y) + length.y * glm::abs(edges[i].x);
        float p1 = -verts[(2 + i) % 3].x * edges[i].y + verts[(2 + i) % 3].y * edges[i].x;
        float p2 = -verts[(3 + i) % 3].x * edges[i].y + verts[(3 + i) % 3].y * edges[i].x;
        if (glm::max(p1, p2) < -r || glm::min(p1, p2) > r) {
            // Separating axis found
            return;
        }
    }

    // testing triangle's AABB with given AABB
    if (glm::max(glm::max(verts[0].x, verts[1].x), verts[2].x) < -length.x ||
            glm::min(glm::min(verts[0].x, verts[1].x), verts[2].x) > length.x) {
        return;
    }
    if (glm::max(glm::max(verts[0].y, verts[1].y), verts[2].y) < -length.y ||
            glm::min(glm::min(verts[0].y, verts[1].y), verts[2].y) > length.y) {
        return;
    }
    if (glm::max(glm::max(verts[0].z, verts[1].z), verts[2].z) < -length.z ||
            glm::min(glm::min(verts[0].z, verts[1].z), verts[2].z) > length.z) {
        return;
    }

    auto p = Plane(glm::cross(edges[0], edges[1]), verts[0]);
    return CollidePrimitive(aabb, p, manifold);
}

void CollidePrimitive(Sphere s1, Sphere s2, CollisionManifold *manifold) {
    manifold->isCollide =  glm::length2(s1.center - s2.center)
        <= (s1.radius + s2.radius) * (s1.radius + s2.radius);
    if (!manifold->isCollide)
        return;
    float r = s1.radius + s2.radius;
    Vec3 d = s1.center - s2.center;
    manifold->normal = Norm(d);
    manifold->penetrationDistance = fabsf(glm::length(d) - r) * 0.5f;
    // dtp - Distance to intersection point
    float dtp = s1.radius - manifold->penetrationDistance;
    Vec3 contact = s1.center + d * dtp;
    manifold->collisionPoint = contact;
}

void CollidePrimitive(AABB aabb, Sphere s, CollisionManifold *manifold) {
    CollidePrimitive(s, aabb, manifold);
    manifold->normal *= -1;
}

void CollidePrimitive(Sphere s, AABB aabb, CollisionManifold *manifold) {
    auto distanceSq = aabb.Distance2(s.center);
    Vec3 p = aabb.ClosestPoint(s.center);

    manifold->isCollide = distanceSq <= s.radius * s.radius;

    if (isCloseToZero(distanceSq)) {
        auto aabbCenter = (aabb.max + aabb.min) / 2.f;
        float mSq = glm::length(p - aabbCenter);
        if (isCloseToZero(mSq))  // here manifold can be strange
            return;
        // Closest point is at the center of the sphere
        manifold->normal = Norm(p - aabbCenter);
    } else {
        manifold->normal = Norm(s.center - p);
    }

    Vec3 outsidePoint = s.center - manifold->normal * s.radius;
    float distance = glm::length(p - outsidePoint);
    manifold->collisionPoint = p + (outsidePoint - p) * 0.5f;
    manifold->penetrationDistance = distance * 0.5f;
}

void CollidePrimitive(Sphere, Triangle, CollisionManifold *manifold);
void CollidePrimitive(Triangle t, Sphere s, CollisionManifold *manifold) {
    CollidePrimitive(s, t, manifold);
}

void CollidePrimitive(Sphere s, Triangle t, CollisionManifold *manifold) {
    manifold->isCollide = t.Distance2(s.center) <= s.radius * s.radius;
}

void CollidePrimitive(Triangle t1, Triangle t2, CollisionManifold *manifold) {
    Plane aPlane = Plane(t1);
    float sdistb[3] = {
        glm::dot(aPlane.normal, t2.a) + aPlane.d,
        glm::dot(aPlane.normal, t2.b) + aPlane.d,
        glm::dot(aPlane.normal, t2.c) + aPlane.d,
    };

    if (glm::abs(sdistb[0]) == 0 && glm::abs(sdistb[1]) == 0 && glm::abs(sdistb[2]) == 0) {
        Logger::Error("Coplanar case for triangle-triangle collision is not handled");
        return;
    }

    if (sdistb[0] * sdistb[1] > 0 && sdistb[1] * sdistb[2] > 0) {
        return;
    }
    Plane bPlane = Plane(t2);
    float sdista[3] = {
        glm::dot(bPlane.normal, t1.a) + bPlane.d,
        glm::dot(bPlane.normal, t1.b) + bPlane.d,
        glm::dot(bPlane.normal, t1.c) + bPlane.d,
    };

    if (glm::abs(sdista[0]) == 0 && glm::abs(sdista[1]) == 0 && glm::abs(sdista[2]) == 0) {
        Logger::Error("Coplanar case for triangle-triangle collision is not handled");
        return;
    }

    if (sdista[0] * sdista[1] > 0 && sdista[1] * sdista[2] > 0) {
        return;
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
    auto [a1, a2] = findInterval(sdista, &t1.a);
    auto [a3, a4] = findInterval(sdistb, &t2.a);
    manifold->isCollide = (glm::max(a1, a2) > a3 && a3 > glm::min(a1, a2))
        || (glm::max(a1, a2) > a4 && a4 > glm::min(a1, a2));
}

// There should be an overload CollidePrimitive(T, Triangle);
template<typename T>
void CollideMeshAt(T t, Mesh *mesh, Transform transform, CollisionManifold *manifold) {
    // WARNING: This makes assumptions about data layout
    Mat4 meshMat = glm::transpose(transform.GetTransformMatrix());
    auto loadPos = [=](int i) {
        int id = mesh->getIndices()[i];
        Vec4 res = Vec4 {mesh->getPoints()[id].Position, 1.0} * meshMat;

        return Vec3{ res.x / res.w, res.y / res.w, res.z / res.w };
    };
    for (int i = 0; i < mesh->getLenIndices(); i+=3) {
        Triangle tri = Triangle(loadPos(i), loadPos(i + 1), loadPos(i + 2));
        CollidePrimitive(t, tri, manifold);
        if (manifold->isCollide) {
            return;
        }
    }
    return;
}
template void CollideMeshAt<AABB>(AABB, Mesh *, Transform, CollisionManifold*);
template void CollideMeshAt<Sphere>(Sphere, Mesh *, Transform, CollisionManifold*);
template void CollideMeshAt<Triangle>(Triangle, Mesh *, Transform, CollisionManifold*);
template void CollideMeshAt<OBB>(OBB, Mesh *, Transform, CollisionManifold*);

void CollideMeshes(Mesh *mesh, Transform transform, Mesh *mesh2,
        Transform transform2, CollisionManifold *manifold) {
    // WARNING: This makes assumptions about data layout
    Mat4 meshMat = glm::transpose(transform.GetTransformMatrix());
    auto loadPos = [=](int i) {
        int id = mesh->getIndices()[i];
        Vec4 res = Vec4 {mesh->getPoints()[id].Position, 1.0} * meshMat;

        return Vec3{ res.x / res.w, res.y / res.w, res.z / res.w };
    };
    for (int i = 0; i < mesh->getLenIndices(); i+=3) {
        Triangle tri = Triangle(loadPos(i), loadPos(i + 1), loadPos(i + 2));
        CollideMeshAt(tri, mesh2, transform2, manifold);
        if (manifold->isCollide) {
            return;
        }
    }
    return;
}

bool CollidePrimitive(Ray r, Sphere s) {
    Vec3 centeredOrigin = r.origin - s.center;
    float b = glm::dot(r.direction, centeredOrigin);
    float c = glm::dot(centeredOrigin, centeredOrigin) - s.radius * s.radius;

    if (c > 0 && b > 0)
        return false;

    return b*b - c >= 0;
}

bool CollidePrimitive(Ray r, AABB aabb) {
    float tmin = 0.f;
    float tmax = std::numeric_limits<float>::max();
    for (int i = 0; i < 3; i++) {
        if (glm::abs(r.direction[i]) < EPS) {
            if (r.origin[i] < aabb.min[i] || r.origin[i] > aabb.max[i]) {
                return false;
            }
            continue;
        }
        float t0 = (aabb.min[i] - r.origin[i]) / r.direction[i];
        float t1 = (aabb.max[i] - r.origin[i]) / r.direction[i];
        if (t0 > t1) {
            float tmp = t1;
            t1 = t0;
            t0 = tmp;
        }
        tmin = glm::max(tmin, t0);
        tmax = glm::min(tmax, t1);

        if (tmin > tmax)
            return false;
    }
    return true;
}

std::optional<float> CollisionPrimitive(Ray r, AABB aabb) {
    float tmin = 0.f;
    float tmax = std::numeric_limits<float>::max();
    for (int i = 0; i < 3; i++) {
        if (glm::abs(r.direction[i]) < EPS) {
            if (r.origin[i] < aabb.min[i] || r.origin[i] > aabb.max[i]) {
                return {};
            }
            continue;
        }
        float t0 = (aabb.min[i] - r.origin[i]) / r.direction[i];
        float t1 = (aabb.max[i] - r.origin[i]) / r.direction[i];
        if (t0 > t1) {
            float tmp = t1;
            t1 = t0;
            t0 = tmp;
        }
        tmin = glm::max(tmin, t0);
        tmax = glm::min(tmax, t1);

        if (tmin > tmax)
            return {};
    }
    return tmin;
}

std::optional<float> CollisionPrimitive(Ray r, Sphere s) {
    Vec3 centeredOrigin = r.origin - s.center;
    float b = glm::dot(r.direction, centeredOrigin);
    float c = glm::dot(centeredOrigin, centeredOrigin) - s.radius * s.radius;

    if (c > 0 && b > 0)
        return {};

    float x = -b - glm::sqrt(b*b - c);
    if (x < 0)
        x = 0;

    return x;
}

std::optional<float> CollisionPrimitive(Ray r, OBB o) {
    const float epsilon = 0.000001;
    Vec3 p = o.center - r.origin;

    Vec3 f(
        glm::dot(o.axis[0], r.direction),
        glm::dot(o.axis[1], r.direction),
        glm::dot(o.axis[2], r.direction));

    Vec3 e(
        glm::dot(o.axis[0], p),
        glm::dot(o.axis[1], p),
        glm::dot(o.axis[2], p));

    float t[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 3; i++) {
        if (std::fabs(f[i]) < epsilon) {
            if (-e[i] - o.axis[0][i] > 0 || -e[i] + o.axis[0][i] < 0) {
                return {};
            }
            f[i] = 0.00001f;  // Avoid div by 0!
        }
        t[i * 2 + 0] = (e[i] + o.axis[0][i]) / f[i];  // min
        t[i * 2 + 1] = (e[i] - o.axis[0][i]) / f[i];  // max
    }

    float tmin = fmaxf(
        fmaxf(
            fminf(t[0], t[1]),
            fminf(t[2], t[3])),
            fminf(t[4], t[5]));

    float tmax = fminf(
        fminf(
            fmaxf(t[0], t[1]),
            fmaxf(t[2], t[3])),
        fmaxf(t[4], t[5]));

    if (tmax < 0) {
        return {};
    }

    if (tmin > tmax) {
        return {};
    }

    if (tmin < 0.0f) {
        return tmax;
    }

    return tmin;
}

