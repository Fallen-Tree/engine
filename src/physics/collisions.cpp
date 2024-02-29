#include <variant>
#include <optional>
#include "logger.hpp"
#include "math_types.hpp"
#include "collisions.hpp"
#include "engine_config.hpp"
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

bool CollidePrimitive(Plane p, AABB a) {
    return CollidePrimitive(a, p);
}

bool CollidePrimitive(AABB aabb, Plane plane) {
    Vec3 center = (aabb.min + aabb.max) / 2.0f;
    Vec3 extents = (aabb.max - aabb.min) / 2.0f;

    float r = extents.x * glm::abs(plane.normal.x) +
        extents.y * glm::abs(plane.normal.y) +
        extents.z * glm::abs(plane.normal.z);

    float c_dist = glm::dot(center, plane.normal) + plane.d;
    return glm::abs(c_dist) <= r;
}

bool CollidePrimitive(AABB lhs, AABB rhs) {
    return lhs.max.x >= rhs.min.x && rhs.max.x >= lhs.min.x &&
        lhs.max.y >= rhs.min.y && rhs.max.y >= lhs.min.y &&
        lhs.max.z >= rhs.min.z && rhs.max.z >= lhs.min.z;
}

bool CollidePrimitive(Triangle t, AABB a) {
    return CollidePrimitive(a, t);
}

bool CollidePrimitive(AABB aabb, Triangle tri) {
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
        float r = length.x * glm::abs(edges[i].y) + length.y * glm::abs(edges[i].x);
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

    auto p = Plane(glm::cross(edges[0], edges[1]), verts[0]);
    return CollidePrimitive(aabb, p);
}

bool CollidePrimitive(Sphere s1, Sphere s2) {
    return glm::length2(s1.center - s2.center) <= (s1.radius + s2.radius) * (s1.radius + s2.radius);
}

bool CollidePrimitive(AABB aabb, Sphere s) {
    return CollidePrimitive(s, aabb);
}

bool CollidePrimitive(Sphere s, AABB aabb) {
    return aabb.Distance2(s.center) <= s.radius * s.radius;
}

bool CollidePrimitive(Sphere, Triangle);
bool CollidePrimitive(Triangle t, Sphere s) {
    return CollidePrimitive(s, t);
}

bool CollidePrimitive(Sphere s, Triangle t) {
    return t.Distance2(s.center) <= s.radius * s.radius;
}

bool CollidePrimitive(Triangle t1, Triangle t2) {
    Plane aPlane = Plane(t1);
    float sdistb[3] = {
        glm::dot(aPlane.normal, t2.a) + aPlane.d,
        glm::dot(aPlane.normal, t2.b) + aPlane.d,
        glm::dot(aPlane.normal, t2.c) + aPlane.d,
    };

    if (glm::abs(sdistb[0]) == 0 && glm::abs(sdistb[1]) == 0 && glm::abs(sdistb[2]) == 0) {
        Logger::Error("Coplanar case for triangle-triangle collision is not handled");
        return false;
    }

    if (sdistb[0] * sdistb[1] > 0 && sdistb[1] * sdistb[2] > 0) {
        return false;
    }
    Plane bPlane = Plane(t2);
    float sdista[3] = {
        glm::dot(bPlane.normal, t1.a) + bPlane.d,
        glm::dot(bPlane.normal, t1.b) + bPlane.d,
        glm::dot(bPlane.normal, t1.c) + bPlane.d,
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
    auto [a1, a2] = findInterval(sdista, &t1.a);
    auto [a3, a4] = findInterval(sdistb, &t2.a);
    return (glm::max(a1, a2) > a3 && a3 > glm::min(a1, a2))
        || (glm::max(a1, a2) > a4 && a4 > glm::min(a1, a2));
}

// There should be an overload CollidePrimitive(T, Triangle);
template<typename T>
bool CollideModelAt(T t, Model *model, Transform transform) {
    // WARNING: This makes assumptions about data layout
    Mat4 modelMat = glm::transpose(transform.GetTransformMatrix());
    int stride = 8;
    auto loadPos = [=](int i) {
        int id = model->getIndices()[i];
        Vec4 res = Vec4 {
            model->getPoints()[id * stride],
            model->getPoints()[id * stride + 1],
            model->getPoints()[id * stride + 2],
            1.0
        } * modelMat;
        return Vec3{ res.x / res.w, res.y / res.w, res.z / res.w };
    };
    for (int i = 0; i < model->getLenIndices(); i+=3) {
        Triangle tri = Triangle(loadPos(i), loadPos(i + 1), loadPos(i + 2));
        if (CollidePrimitive(t, tri)) {
            return true;
        }
    }
    return false;
}
template bool CollideModelAt<AABB>(AABB, Model *, Transform);
template bool CollideModelAt<Sphere>(Sphere, Model *, Transform);
template bool CollideModelAt<Triangle>(Triangle, Model *, Transform);

bool CollideModels(Model *model, Transform transform, Model *model2, Transform transform2) {
    // WARNING: This makes assumptions about data layout
    Mat4 modelMat = glm::transpose(transform.GetTransformMatrix());
    int stride = 8;
    auto loadPos = [=](int i) {
        int id = model->getIndices()[i];
        Vec4 res = Vec4 {
            model->getPoints()[id * stride],
            model->getPoints()[id * stride + 1],
            model->getPoints()[id * stride + 2],
            1.0
        } * modelMat;
        return Vec3{ res.x / res.w, res.y / res.w, res.z / res.w };
    };
    for (int i = 0; i < model->getLenIndices(); i+=3) {
        Triangle tri = Triangle(loadPos(i), loadPos(i + 1), loadPos(i + 2));
        if (CollideModelAt(tri, model2, transform2)) {
            return true;
        }
    }
    return false;
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

Vec3 CollisionNormal(AABB a1, AABB a2, Transform tr1, Transform tr2, Vec3 velocity, float dt) {
    const float epsilon = 0.1;
    auto transformed1 = a1.Transformed(tr1).PrevState(velocity, dt);
    auto transformed2 = a2.Transformed(tr2);

    if (transformed1.min.x + epsilon >= transformed2.max.x) {
        return Vec3(1, 0, 0);
    } else if (transformed1.max.x <= transformed2.min.x + epsilon) {
        return Vec3(-1, 0, 0);
    } else if (transformed1.min.y + epsilon >= transformed2.max.y) {
        return Vec3(0, 1, 0);
    } else if (transformed1.max.y <= transformed2.min.y + epsilon) {
        return Vec3(0, -1, 0);
    } else if (transformed1.min.z + epsilon >= transformed2.max.z) {
        return Vec3(0, 0, 1);
    } else if (transformed1.max.z <= transformed2.min.z + epsilon) {
        return Vec3(0, 0, -1);
    }

    // the case if one object is inside other
    return Norm(tr1.GetTranslation() - tr2.GetTranslation())
        * float(EJECTION_RATIO);
}

Vec3 CollisionNormal(Sphere sph, AABB a, Transform tr1, Transform tr2,
        Vec3 velocity, float dt) {
    return -CollisionNormal(a, sph, tr2, tr1, velocity, dt);
}

Vec3 CollisionNormal(AABB a, Sphere sph, Transform tr1, Transform tr2,
        Vec3 velocity, float dt) {
    const float epsilon = 0.1;
    auto transformed2 = sph.Transformed(tr2);
    auto transformed1 = a.Transformed(tr1).PrevState(velocity, dt);

    auto closetPoint = transformed2.ClosestPoint(
            (transformed1.min + transformed1.max) * 0.5f);

    Logger::Info("\nmax %s\nmin %s\npoint %s\n",
            glm::to_string(transformed1.max).c_str(),
            glm::to_string(transformed1.min).c_str(),
            glm::to_string(closetPoint).c_str());

    if (closetPoint.x + epsilon >= transformed1.max.x) {
        return Vec3(-1, 0, 0);
    } else if (closetPoint.x <= transformed1.min.x + epsilon) {
        return Vec3(1, 0, 0);
    } else if (closetPoint.y + epsilon >= transformed1.max.y) {
        return Vec3(0, -1, 0);
    } else if (closetPoint.y <= transformed1.min.y + epsilon) {
        return Vec3(0, 1, 0);
    } else if (closetPoint.z + epsilon >= transformed1.max.z) {
        return Vec3(0, 0, -1);
    } else if (closetPoint.z <= transformed1.min.z + epsilon) {
        return Vec3(0, 0, 1);
   }

    Logger::Info("!");

    // the case if one object is inside other
    return Norm(tr1.GetTranslation() - tr2.GetTranslation())
        * float(EJECTION_RATIO);
}

Vec3 CollisionNormal(Sphere sph1, Sphere sph2,
        Transform tr1, Transform tr2, Vec3 vel, float dt) {
    auto tranformed1 = sph1.Transformed(tr1);
    auto tranformed2 = sph2.Transformed(tr2);

    return Norm(tranformed1.center - tranformed2.center);
}

// TODO(solloballon): make this operation with Model
Vec3 CollisionNormal(AABB, Model*, Transform, Transform, Vec3, float) {
    Logger::Warn(
            "COLLISIONS::COLLISION_NORMAL::THERE_IS_NO_DEFINITION_OPERATION_MODEL");
    return Vec3(0);
}

Vec3 CollisionNormal(Model*, AABB, Transform, Transform, Vec3, float) {
    Logger::Warn(
            "COLLISIONS::COLLISION_NORMAL::THERE_IS_NO_DEFINITION_OPERATION_MODEL");
    return Vec3(0);
}

Vec3 CollisionNormal(Sphere, Model*, Transform, Transform, Vec3, float) {
    Logger::Warn(
            "COLLISIONS::COLLISION_NORMAL::THERE_IS_NO_DEFINITION_OPERATION_MODEL");
    return Vec3(0);
}

Vec3 CollisionNormal(Model*, Sphere, Transform, Transform, Vec3, float) {
    Logger::Warn(
            "COLLISIONS::COLLISION_NORMAL::THERE_IS_NO_DEFINITION_OPERATION_MODEL");
    return Vec3(0);
}

Vec3 CollisionNormal(Model*, Model*, Transform, Transform, Vec3, float) {
    Logger::Warn(
            "COLLISIONS::COLLISION_NORMAL::THERE_IS_NO_DEFINITION_OPERATION_MODEL");
    return Vec3(0);
}
