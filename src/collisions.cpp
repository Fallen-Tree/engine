#include <typeinfo>
#include "logger.hpp"
#include "math_types.hpp"
#include "collisions.hpp"
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>

bool CollidePrimitive(AABB, Plane);
bool CollidePrimitive(Plane p, AABB a) { return CollidePrimitive(a, p); }
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

bool CollidePrimitive(AABB, Triangle);
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

    return CollidePrimitive(aabb, Plane(tri));
}

bool CollidePrimitive(Sphere s1, Sphere s2) {
    return glm::length2(s1.center - s2.center) <= (s1.radius + s2.radius) * (s1.radius + s2.radius);
}

bool CollidePrimitive(Sphere, AABB);
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

template<typename T>
bool CollidePrimitive(Model *model, T t, Transform transform) {
    return CollidePrimitive(t, model, transform);
}

// There should be an overload CollidePrimitive(T, Triangle);
template<typename T>
bool CollidePrimitive(T t, Model *model, Transform transform) {
    // WARNING: This makes assumptions about data layout
    Mat3 modelMat = transform.GetTransformMatrix();
    int stride = 8;
    auto loadPos = [=](Model *model, int i) {
        int id = model->getIndices()[i];
        return Vec3 {
            model->getPoints()[id * stride],
            model->getPoints()[id * stride + 1],
            model->getPoints()[id * stride + 2],
        } * modelMat;
    };
    for (int i = 0; i < model->getLenIndices(); i+=3) {
        Triangle tri = Triangle(
            loadPos(model, i),
            loadPos(model, i + 1),
            loadPos(model, i + 2));
        if (CollidePrimitive(t, tri)) {
            return true;
        }
    }
    return false;
}

template<typename... Base>
struct Visitor : Base... {
    using Base::operator()...;
};

template<typename... T>
Visitor(T...) -> Visitor<T...>;

template<typename R, typename Variant, typename... Lambdas>
R match(Variant &&variant, Lambdas &&... lambdas) {
    return std::visit(Visitor(lambdas...), variant);
}

template<typename R, typename Variant, typename Visitor>
R match2(Variant &&variant1, Variant &&variant2, Visitor &&visitor) {
    return match(variant1,
        [=](auto var1) {
            match(variant2,
                [=](auto var2) {
                    visitor(var1, var2);
                });
        });
}

bool Collider::Collide(AABB lhs, Transform lhsTransform, AABB rhs, Transform rhsTransform) {
    auto lhsGlobal = AABB {
        lhs.min + lhsTransform.GetTranslation(),
        lhs.max + lhsTransform.GetTranslation(),
    };
    auto rhsGlobal = AABB {
        rhs.min + rhsTransform.GetTranslation(),
        rhs.max + rhsTransform.GetTranslation(),
    };

    return CollidePrimitive(lhsGlobal, rhsGlobal);
}

bool Collider::Collide(Sphere lhs, Transform lhsTransform, AABB rhs, Transform rhsTransform) {
    return Collide(rhs, rhsTransform, lhs, lhsTransform);
}

bool Collider::Collide(AABB lhs, Transform lhsTransform, Sphere rhs, Transform rhsTransform) {
    auto lhsGlobal = AABB {
        lhs.min + lhsTransform.GetTranslation(),
        lhs.max + lhsTransform.GetTranslation(),
    };
    auto rhsGlobal = Sphere {
        rhs.center + rhsTransform.GetTranslation(),
        rhs.radius,
    };

    return CollidePrimitive(lhsGlobal, rhsGlobal);
}

bool Collider::Collide(Sphere lhs, Transform lhsTransform, Sphere rhs, Transform rhsTransform) {
    auto lhsGlobal = Sphere {
        lhs.center + lhsTransform.GetTranslation(),
        lhs.radius,
    };
    auto rhsGlobal = Sphere {
        rhs.center + rhsTransform.GetTranslation(),
        rhs.radius,
    };

    return CollidePrimitive(lhsGlobal, rhsGlobal);
}

bool Collider::Collide(AABB lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform) {
    return Collide(rhs, rhsTransform, lhs, lhsTransform);
}

bool Collider::Collide(Model *lhs, Transform lhsTransform, AABB rhs, Transform rhsTransform) {
    auto rhsGlobal = AABB {
        rhs.min + rhsTransform.GetTranslation(),
        rhs.max + rhsTransform.GetTranslation(),
    };

    return CollidePrimitive(rhsGlobal, lhs, lhsTransform);
}

bool Collider::Collide(Sphere lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform) {
    return Collide(rhs, rhsTransform, lhs, lhsTransform);
}

bool Collider::Collide(Model *lhs, Transform lhsTransform, Sphere rhs, Transform rhsTransform) {
    auto rhsGlobal = Sphere {
        rhs.center + rhsTransform.GetTranslation(),
        rhs.radius,
    };

    return CollidePrimitive(rhsGlobal, lhs, lhsTransform);
}

bool Collider::Collide(Model *lhs, Transform lhsTransform, Model *rhs, Transform rhsTransform) {
    // WARNING: This makes assumptions about data layout
    Mat3 modelMat = lhsTransform.GetTransformMatrix();
    int stride = 8;
    auto loadPos = [=](Model *model, int i) {
        int id = model->getIndices()[i];
        return Vec3 {
            model->getPoints()[id * stride],
            model->getPoints()[id * stride + 1],
            model->getPoints()[id * stride + 2],
        } * modelMat;
    };
    for (int i = 0; i < lhs->getLenIndices(); i+=3) {
        Triangle tri = Triangle(
            loadPos(lhs, i),
            loadPos(lhs, i + 1),
            loadPos(lhs, i + 2));
        if (CollidePrimitive(tri, rhs, rhsTransform)) {
            return true;
        }
    }
    return false;
}

bool Collider::Collide(Transform self, Collider *other, Transform otherTransform) {
    return std::visit([=](auto var1) {
            return std::visit(
                [=](auto var2) {
                    return this->Collide(var1, self, var2, otherTransform);
                }, other->shape);
        }, shape);
}
