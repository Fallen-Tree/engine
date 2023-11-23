#pragma once

#include "math_types.hpp"

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
