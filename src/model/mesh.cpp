#include "mesh.hpp"

#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdarg>

#include "math_types.hpp"
#include "geometry_primitives.hpp"

/* Constructors */
Mesh::Mesh(const std::vector<Vertex>& Points, const std::vector<unsigned int>& Indices) {
    setPoints(Points);
    setIndices(Indices);
}

Mesh::Mesh(const std::vector<Vertex>& Points) {
    setPoints(Points);
    setDefaultIndices();
}

/* Getters */
Vertex* Mesh::getPoints() {
    assert(this->points.size() != 0);
    return this->points.data();
}

std::vector<Vertex>& Mesh::getVecPoints() {
    return points;
}

unsigned int* Mesh::getIndices() {
    assert(indices.size() != 0);
    return this->indices.data();
}

std::vector<unsigned int>& Mesh::getVecIndices() {
    return indices;
}

int Mesh::getLenIndices() {
    return static_cast<unsigned int>(indices.size());
}

int Mesh::getLenArrPoints() {
    return static_cast<unsigned int>(points.size());
}


/* Setters */
void Mesh::setPoints(const std::vector<Vertex>& points) {
    this->points.assign(points.begin(), points.end());
}

void Mesh::setIndices(const std::vector<unsigned int>& indices) {
    this->indices.assign(indices.begin(), indices.end());
}

void Mesh::setDefaultIndices() {
    indices.resize(points.size());
    for (unsigned int i = 0; i < points.size(); i++) {
        indices[i] = i;
    }
}


/* Utils  */
Vec3 Mesh::ClosestPoint(Vec3 point, Transform transform) {
    Mat4 modelMat = transform.GetTransformMatrix();

    auto loadPos = [=](int i) {
        int id = getIndices()[i];
        Vec4 res = Vec4 {getPoints()[id].Position, 1.0} * modelMat;
        return Vec3{ res.x / res.w, res.y / res.w, res.z / res.w };
    };

    Vec3 res = Vec3(std::numeric_limits<float>::max());
    for (int i = 0; i < getLenIndices(); i += 3) {
        Triangle tri = Triangle(loadPos(i), loadPos(i + 1), loadPos(i + 2));
        Vec3 p = tri.ClosestPoint(point);
        if (glm::length(p) < glm::length(res))
            res = p;
    }
    return res;
}

Vec3 Mesh::CollisionNormal(Vec3 point, Transform transform) {
    const int stride = 8;
    Mat4 modelMat = transform.GetTransformMatrix();

    auto loadPos = [=](int i) {
        int id = getIndices()[i];
        Vec4 res = Vec4 {getPoints()[id].Position, 1.0} * modelMat;
        return Vec3{ res.x / res.w, res.y / res.w, res.z / res.w };
    };

    float min = std::numeric_limits<float>::max();

    Triangle *res;
    for (int i = 0; i < getLenIndices(); i += 3) {
        Triangle tri = Triangle(loadPos(i), loadPos(i + 1), loadPos(i + 2));
        float dist = tri.Distance2(point);
        if (dist < min) {
            min = dist;
            *res = tri;
        }
    }

    return -Norm((*res).normal);
    // Is ok direction or i should make smthg with this ?
}

Mesh *Mesh::GetSphere() {
    std::vector<Vec3> points = {{0.000, 0.000, -1.000}, {0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {0.707, 0.000, -0.707}, {0.383, 0.000, -0.924}, {0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {0.408, 0.408, -0.816}, {0.000, 0.383, -0.924}, {0.408, 0.408, -0.816}, {0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, -0.383}, {0.924, 0.383, 0.000}, {0.707, 0.000, -0.707}, {0.924, 0.000, -0.383}, {0.816, 0.408, -0.408}, {0.707, 0.707, 0.000}, {0.816, 0.408, -0.408}, {0.924, 0.383, 0.000}, {0.816, 0.408, -0.408}, {0.924, 0.000, -0.383}, {0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {0.707, 0.707, 0.000}, {0.383, 0.924, 0.000}, {0.408, 0.816, -0.408}, {0.000, 0.707, -0.707}, {0.408, 0.816, -0.408}, {0.000, 0.924, -0.383}, {0.408, 0.816, -0.408}, {0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {0.707, 0.707, 0.000}, {0.816, 0.408, -0.408}, {0.408, 0.816, -0.408}, {0.707, 0.000, -0.707}, {0.816, 0.408, -0.408}, {0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {0.408, 0.408, -0.816}, {0.408, 0.816, -0.408}, {0.408, 0.408, -0.816}, {0.816, 0.408, -0.408}, {0.408, 0.816, -0.408}, {0.000, 0.000, -1.000}, {-0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {-0.707, 0.000, -0.707}, {-0.383, 0.000, -0.924}, {-0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {-0.408, 0.408, -0.816}, {0.000, 0.383, -0.924}, {-0.408, 0.408, -0.816}, {-0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, -0.383}, {-0.924, 0.383, 0.000}, {-0.707, 0.000, -0.707}, {-0.924, 0.000, -0.383}, {-0.816, 0.408, -0.408}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, -0.408}, {-0.924, 0.383, 0.000}, {-0.816, 0.408, -0.408}, {-0.924, 0.000, -0.383}, {-0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {-0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {-0.707, 0.707, 0.000}, {-0.383, 0.924, 0.000}, {-0.408, 0.816, -0.408}, {0.000, 0.707, -0.707}, {-0.408, 0.816, -0.408}, {0.000, 0.924, -0.383}, {-0.408, 0.816, -0.408}, {-0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, -0.408}, {-0.408, 0.816, -0.408}, {-0.707, 0.000, -0.707}, {-0.816, 0.408, -0.408}, {-0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {-0.408, 0.408, -0.816}, {-0.408, 0.816, -0.408}, {-0.408, 0.408, -0.816}, {-0.816, 0.408, -0.408}, {-0.408, 0.816, -0.408}, {0.000, 0.000, -1.000}, {0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {0.707, 0.000, -0.707}, {0.383, 0.000, -0.924}, {0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {0.408, -0.408, -0.816}, {0.000, -0.383, -0.924}, {0.408, -0.408, -0.816}, {0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, -0.383}, {0.924, -0.383, 0.000}, {0.707, 0.000, -0.707}, {0.924, 0.000, -0.383}, {0.816, -0.408, -0.408}, {0.707, -0.707, 0.000}, {0.816, -0.408, -0.408}, {0.924, -0.383, 0.000}, {0.816, -0.408, -0.408}, {0.924, 0.000, -0.383}, {0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {0.707, -0.707, 0.000}, {0.383, -0.924, 0.000}, {0.408, -0.816, -0.408}, {0.000, -0.707, -0.707}, {0.408, -0.816, -0.408}, {0.000, -0.924, -0.383}, {0.408, -0.816, -0.408}, {0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {0.707, -0.707, 0.000}, {0.816, -0.408, -0.408}, {0.408, -0.816, -0.408}, {0.707, 0.000, -0.707}, {0.816, -0.408, -0.408}, {0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {0.408, -0.408, -0.816}, {0.408, -0.816, -0.408}, {0.408, -0.408, -0.816}, {0.816, -0.408, -0.408}, {0.408, -0.816, -0.408}, {0.000, 0.000, -1.000}, {-0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {-0.707, 0.000, -0.707}, {-0.383, 0.000, -0.924}, {-0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {-0.408, -0.408, -0.816}, {0.000, -0.383, -0.924}, {-0.408, -0.408, -0.816}, {-0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, -0.383}, {-0.924, -0.383, 0.000}, {-0.707, 0.000, -0.707}, {-0.924, 0.000, -0.383}, {-0.816, -0.408, -0.408}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, -0.408}, {-0.924, -0.383, 0.000}, {-0.816, -0.408, -0.408}, {-0.924, 0.000, -0.383}, {-0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {-0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {-0.707, -0.707, 0.000}, {-0.383, -0.924, 0.000}, {-0.408, -0.816, -0.408}, {0.000, -0.707, -0.707}, {-0.408, -0.816, -0.408}, {0.000, -0.924, -0.383}, {-0.408, -0.816, -0.408}, {-0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, -0.408}, {-0.408, -0.816, -0.408}, {-0.707, 0.000, -0.707}, {-0.816, -0.408, -0.408}, {-0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {-0.408, -0.408, -0.816}, {-0.408, -0.816, -0.408}, {-0.408, -0.408, -0.816}, {-0.816, -0.408, -0.408}, {-0.408, -0.816, -0.408}, {0.000, 0.000, 1.000}, {0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {0.707, 0.000, 0.707}, {0.383, 0.000, 0.924}, {0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {0.408, 0.408, 0.816}, {0.000, 0.383, 0.924}, {0.408, 0.408, 0.816}, {0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, 0.383}, {0.924, 0.383, 0.000}, {0.707, 0.000, 0.707}, {0.924, 0.000, 0.383}, {0.816, 0.408, 0.408}, {0.707, 0.707, 0.000}, {0.816, 0.408, 0.408}, {0.924, 0.383, 0.000}, {0.816, 0.408, 0.408}, {0.924, 0.000, 0.383}, {0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {0.707, 0.707, 0.000}, {0.383, 0.924, 0.000}, {0.408, 0.816, 0.408}, {0.000, 0.707, 0.707}, {0.408, 0.816, 0.408}, {0.000, 0.924, 0.383}, {0.408, 0.816, 0.408}, {0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {0.707, 0.707, 0.000}, {0.816, 0.408, 0.408}, {0.408, 0.816, 0.408}, {0.707, 0.000, 0.707}, {0.816, 0.408, 0.408}, {0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {0.408, 0.408, 0.816}, {0.408, 0.816, 0.408}, {0.408, 0.408, 0.816}, {0.816, 0.408, 0.408}, {0.408, 0.816, 0.408}, {0.000, 0.000, 1.000}, {-0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {-0.707, 0.000, 0.707}, {-0.383, 0.000, 0.924}, {-0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {-0.408, 0.408, 0.816}, {0.000, 0.383, 0.924}, {-0.408, 0.408, 0.816}, {-0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, 0.383}, {-0.924, 0.383, 0.000}, {-0.707, 0.000, 0.707}, {-0.924, 0.000, 0.383}, {-0.816, 0.408, 0.408}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, 0.408}, {-0.924, 0.383, 0.000}, {-0.816, 0.408, 0.408}, {-0.924, 0.000, 0.383}, {-0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {-0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {-0.707, 0.707, 0.000}, {-0.383, 0.924, 0.000}, {-0.408, 0.816, 0.408}, {0.000, 0.707, 0.707}, {-0.408, 0.816, 0.408}, {0.000, 0.924, 0.383}, {-0.408, 0.816, 0.408}, {-0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, 0.408}, {-0.408, 0.816, 0.408}, {-0.707, 0.000, 0.707}, {-0.816, 0.408, 0.408}, {-0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {-0.408, 0.408, 0.816}, {-0.408, 0.816, 0.408}, {-0.408, 0.408, 0.816}, {-0.816, 0.408, 0.408}, {-0.408, 0.816, 0.408}, {0.000, 0.000, 1.000}, {0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {0.707, 0.000, 0.707}, {0.383, 0.000, 0.924}, {0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {0.408, -0.408, 0.816}, {0.000, -0.383, 0.924}, {0.408, -0.408, 0.816}, {0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, 0.383}, {0.924, -0.383, 0.000}, {0.707, 0.000, 0.707}, {0.924, 0.000, 0.383}, {0.816, -0.408, 0.408}, {0.707, -0.707, 0.000}, {0.816, -0.408, 0.408}, {0.924, -0.383, 0.000}, {0.816, -0.408, 0.408}, {0.924, 0.000, 0.383}, {0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {0.707, -0.707, 0.000}, {0.383, -0.924, 0.000}, {0.408, -0.816, 0.408}, {0.000, -0.707, 0.707}, {0.408, -0.816, 0.408}, {0.000, -0.924, 0.383}, {0.408, -0.816, 0.408}, {0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {0.707, -0.707, 0.000}, {0.816, -0.408, 0.408}, {0.408, -0.816, 0.408}, {0.707, 0.000, 0.707}, {0.816, -0.408, 0.408}, {0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {0.408, -0.408, 0.816}, {0.408, -0.816, 0.408}, {0.408, -0.408, 0.816}, {0.816, -0.408, 0.408}, {0.408, -0.816, 0.408}, {0.000, 0.000, 1.000}, {-0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {-0.707, 0.000, 0.707}, {-0.383, 0.000, 0.924}, {-0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {-0.408, -0.408, 0.816}, {0.000, -0.383, 0.924}, {-0.408, -0.408, 0.816}, {-0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, 0.383}, {-0.924, -0.383, 0.000}, {-0.707, 0.000, 0.707}, {-0.924, 0.000, 0.383}, {-0.816, -0.408, 0.408}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, 0.408}, {-0.924, -0.383, 0.000}, {-0.816, -0.408, 0.408}, {-0.924, 0.000, 0.383}, {-0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {-0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {-0.707, -0.707, 0.000}, {-0.383, -0.924, 0.000}, {-0.408, -0.816, 0.408}, {0.000, -0.707, 0.707}, {-0.408, -0.816, 0.408}, {0.000, -0.924, 0.383}, {-0.408, -0.816, 0.408}, {-0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, 0.408}, {-0.408, -0.816, 0.408}, {-0.707, 0.000, 0.707}, {-0.816, -0.408, 0.408}, {-0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {-0.408, -0.408, 0.816}, {-0.408, -0.816, 0.408}, {-0.408, -0.408, 0.816}, {-0.816, -0.408, 0.408}, {-0.408, -0.816, 0.408}};
    std::vector<Vec3> normals = {{0.191, 0.191, -0.963}, {0.191, 0.191, -0.963}, {0.191, 0.191, -0.963}, {0.546, 0.181, -0.818}, {0.546, 0.181, -0.818}, {0.546, 0.181, -0.818}, {0.181, 0.546, -0.818}, {0.181, 0.546, -0.818}, {0.181, 0.546, -0.818}, {0.234, 0.234, -0.944}, {0.234, 0.234, -0.944}, {0.234, 0.234, -0.944}, {0.963, 0.191, -0.191}, {0.963, 0.191, -0.191}, {0.963, 0.191, -0.191}, {0.818, 0.181, -0.546}, {0.818, 0.181, -0.546}, {0.818, 0.181, -0.546}, {0.818, 0.546, -0.181}, {0.818, 0.546, -0.181}, {0.818, 0.546, -0.181}, {0.944, 0.234, -0.234}, {0.944, 0.234, -0.234}, {0.944, 0.234, -0.234}, {0.191, 0.963, -0.191}, {0.191, 0.963, -0.191}, {0.191, 0.963, -0.191}, {0.546, 0.818, -0.181}, {0.546, 0.818, -0.181}, {0.546, 0.818, -0.181}, {0.181, 0.818, -0.546}, {0.181, 0.818, -0.546}, {0.181, 0.818, -0.546}, {0.234, 0.944, -0.234}, {0.234, 0.944, -0.234}, {0.234, 0.944, -0.234}, {0.672, 0.672, -0.312}, {0.672, 0.672, -0.312}, {0.672, 0.672, -0.312}, {0.672, 0.312, -0.672}, {0.672, 0.312, -0.672}, {0.672, 0.312, -0.672}, {0.312, 0.672, -0.672}, {0.312, 0.672, -0.672}, {0.312, 0.672, -0.672}, {0.577, 0.577, -0.577}, {0.577, 0.577, -0.577}, {0.577, 0.577, -0.577}, {-0.191, 0.191, -0.963}, {-0.191, 0.191, -0.963}, {-0.191, 0.191, -0.963}, {-0.546, 0.181, -0.818}, {-0.546, 0.181, -0.818}, {-0.546, 0.181, -0.818}, {-0.181, 0.546, -0.818}, {-0.181, 0.546, -0.818}, {-0.181, 0.546, -0.818}, {-0.234, 0.234, -0.944}, {-0.234, 0.234, -0.944}, {-0.234, 0.234, -0.944}, {-0.963, 0.191, -0.191}, {-0.963, 0.191, -0.191}, {-0.963, 0.191, -0.191}, {-0.818, 0.181, -0.546}, {-0.818, 0.181, -0.546}, {-0.818, 0.181, -0.546}, {-0.818, 0.546, -0.181}, {-0.818, 0.546, -0.181}, {-0.818, 0.546, -0.181}, {-0.944, 0.234, -0.234}, {-0.944, 0.234, -0.234}, {-0.944, 0.234, -0.234}, {-0.191, 0.963, -0.191}, {-0.191, 0.963, -0.191}, {-0.191, 0.963, -0.191}, {-0.546, 0.818, -0.181}, {-0.546, 0.818, -0.181}, {-0.546, 0.818, -0.181}, {-0.181, 0.818, -0.546}, {-0.181, 0.818, -0.546}, {-0.181, 0.818, -0.546}, {-0.234, 0.944, -0.234}, {-0.234, 0.944, -0.234}, {-0.234, 0.944, -0.234}, {-0.672, 0.672, -0.312}, {-0.672, 0.672, -0.312}, {-0.672, 0.672, -0.312}, {-0.672, 0.312, -0.672}, {-0.672, 0.312, -0.672}, {-0.672, 0.312, -0.672}, {-0.312, 0.672, -0.672}, {-0.312, 0.672, -0.672}, {-0.312, 0.672, -0.672}, {-0.577, 0.577, -0.577}, {-0.577, 0.577, -0.577}, {-0.577, 0.577, -0.577}, {0.191, -0.191, -0.963}, {0.191, -0.191, -0.963}, {0.191, -0.191, -0.963}, {0.546, -0.181, -0.818}, {0.546, -0.181, -0.818}, {0.546, -0.181, -0.818}, {0.181, -0.546, -0.818}, {0.181, -0.546, -0.818}, {0.181, -0.546, -0.818}, {0.234, -0.234, -0.944}, {0.234, -0.234, -0.944}, {0.234, -0.234, -0.944}, {0.963, -0.191, -0.191}, {0.963, -0.191, -0.191}, {0.963, -0.191, -0.191}, {0.818, -0.181, -0.546}, {0.818, -0.181, -0.546}, {0.818, -0.181, -0.546}, {0.818, -0.546, -0.181}, {0.818, -0.546, -0.181}, {0.818, -0.546, -0.181}, {0.944, -0.234, -0.234}, {0.944, -0.234, -0.234}, {0.944, -0.234, -0.234}, {0.191, -0.963, -0.191}, {0.191, -0.963, -0.191}, {0.191, -0.963, -0.191}, {0.546, -0.818, -0.181}, {0.546, -0.818, -0.181}, {0.546, -0.818, -0.181}, {0.181, -0.818, -0.546}, {0.181, -0.818, -0.546}, {0.181, -0.818, -0.546}, {0.234, -0.944, -0.234}, {0.234, -0.944, -0.234}, {0.234, -0.944, -0.234}, {0.672, -0.672, -0.312}, {0.672, -0.672, -0.312}, {0.672, -0.672, -0.312}, {0.672, -0.312, -0.672}, {0.672, -0.312, -0.672}, {0.672, -0.312, -0.672}, {0.312, -0.672, -0.672}, {0.312, -0.672, -0.672}, {0.312, -0.672, -0.672}, {0.577, -0.577, -0.577}, {0.577, -0.577, -0.577}, {0.577, -0.577, -0.577}, {-0.191, -0.191, -0.963}, {-0.191, -0.191, -0.963}, {-0.191, -0.191, -0.963}, {-0.546, -0.181, -0.818}, {-0.546, -0.181, -0.818}, {-0.546, -0.181, -0.818}, {-0.181, -0.546, -0.818}, {-0.181, -0.546, -0.818}, {-0.181, -0.546, -0.818}, {-0.234, -0.234, -0.944}, {-0.234, -0.234, -0.944}, {-0.234, -0.234, -0.944}, {-0.963, -0.191, -0.191}, {-0.963, -0.191, -0.191}, {-0.963, -0.191, -0.191}, {-0.818, -0.181, -0.546}, {-0.818, -0.181, -0.546}, {-0.818, -0.181, -0.546}, {-0.818, -0.546, -0.181}, {-0.818, -0.546, -0.181}, {-0.818, -0.546, -0.181}, {-0.944, -0.234, -0.234}, {-0.944, -0.234, -0.234}, {-0.944, -0.234, -0.234}, {-0.191, -0.963, -0.191}, {-0.191, -0.963, -0.191}, {-0.191, -0.963, -0.191}, {-0.546, -0.818, -0.181}, {-0.546, -0.818, -0.181}, {-0.546, -0.818, -0.181}, {-0.181, -0.818, -0.546}, {-0.181, -0.818, -0.546}, {-0.181, -0.818, -0.546}, {-0.234, -0.944, -0.234}, {-0.234, -0.944, -0.234}, {-0.234, -0.944, -0.234}, {-0.672, -0.672, -0.312}, {-0.672, -0.672, -0.312}, {-0.672, -0.672, -0.312}, {-0.672, -0.312, -0.672}, {-0.672, -0.312, -0.672}, {-0.672, -0.312, -0.672}, {-0.312, -0.672, -0.672}, {-0.312, -0.672, -0.672}, {-0.312, -0.672, -0.672}, {-0.577, -0.577, -0.577}, {-0.577, -0.577, -0.577}, {-0.577, -0.577, -0.577}, {0.191, 0.191, 0.963}, {0.191, 0.191, 0.963}, {0.191, 0.191, 0.963}, {0.546, 0.181, 0.818}, {0.546, 0.181, 0.818}, {0.546, 0.181, 0.818}, {0.181, 0.546, 0.818}, {0.181, 0.546, 0.818}, {0.181, 0.546, 0.818}, {0.234, 0.234, 0.944}, {0.234, 0.234, 0.944}, {0.234, 0.234, 0.944}, {0.963, 0.191, 0.191}, {0.963, 0.191, 0.191}, {0.963, 0.191, 0.191}, {0.818, 0.181, 0.546}, {0.818, 0.181, 0.546}, {0.818, 0.181, 0.546}, {0.818, 0.546, 0.181}, {0.818, 0.546, 0.181}, {0.818, 0.546, 0.181}, {0.944, 0.234, 0.234}, {0.944, 0.234, 0.234}, {0.944, 0.234, 0.234}, {0.191, 0.963, 0.191}, {0.191, 0.963, 0.191}, {0.191, 0.963, 0.191}, {0.546, 0.818, 0.181}, {0.546, 0.818, 0.181}, {0.546, 0.818, 0.181}, {0.181, 0.818, 0.546}, {0.181, 0.818, 0.546}, {0.181, 0.818, 0.546}, {0.234, 0.944, 0.234}, {0.234, 0.944, 0.234}, {0.234, 0.944, 0.234}, {0.672, 0.672, 0.312}, {0.672, 0.672, 0.312}, {0.672, 0.672, 0.312}, {0.672, 0.312, 0.672}, {0.672, 0.312, 0.672}, {0.672, 0.312, 0.672}, {0.312, 0.672, 0.672}, {0.312, 0.672, 0.672}, {0.312, 0.672, 0.672}, {0.577, 0.577, 0.577}, {0.577, 0.577, 0.577}, {0.577, 0.577, 0.577}, {-0.191, 0.191, 0.963}, {-0.191, 0.191, 0.963}, {-0.191, 0.191, 0.963}, {-0.546, 0.181, 0.818}, {-0.546, 0.181, 0.818}, {-0.546, 0.181, 0.818}, {-0.181, 0.546, 0.818}, {-0.181, 0.546, 0.818}, {-0.181, 0.546, 0.818}, {-0.234, 0.234, 0.944}, {-0.234, 0.234, 0.944}, {-0.234, 0.234, 0.944}, {-0.963, 0.191, 0.191}, {-0.963, 0.191, 0.191}, {-0.963, 0.191, 0.191}, {-0.818, 0.181, 0.546}, {-0.818, 0.181, 0.546}, {-0.818, 0.181, 0.546}, {-0.818, 0.546, 0.181}, {-0.818, 0.546, 0.181}, {-0.818, 0.546, 0.181}, {-0.944, 0.234, 0.234}, {-0.944, 0.234, 0.234}, {-0.944, 0.234, 0.234}, {-0.191, 0.963, 0.191}, {-0.191, 0.963, 0.191}, {-0.191, 0.963, 0.191}, {-0.546, 0.818, 0.181}, {-0.546, 0.818, 0.181}, {-0.546, 0.818, 0.181}, {-0.181, 0.818, 0.546}, {-0.181, 0.818, 0.546}, {-0.181, 0.818, 0.546}, {-0.234, 0.944, 0.234}, {-0.234, 0.944, 0.234}, {-0.234, 0.944, 0.234}, {-0.672, 0.672, 0.312}, {-0.672, 0.672, 0.312}, {-0.672, 0.672, 0.312}, {-0.672, 0.312, 0.672}, {-0.672, 0.312, 0.672}, {-0.672, 0.312, 0.672}, {-0.312, 0.672, 0.672}, {-0.312, 0.672, 0.672}, {-0.312, 0.672, 0.672}, {-0.577, 0.577, 0.577}, {-0.577, 0.577, 0.577}, {-0.577, 0.577, 0.577}, {0.191, -0.191, 0.963}, {0.191, -0.191, 0.963}, {0.191, -0.191, 0.963}, {0.546, -0.181, 0.818}, {0.546, -0.181, 0.818}, {0.546, -0.181, 0.818}, {0.181, -0.546, 0.818}, {0.181, -0.546, 0.818}, {0.181, -0.546, 0.818}, {0.234, -0.234, 0.944}, {0.234, -0.234, 0.944}, {0.234, -0.234, 0.944}, {0.963, -0.191, 0.191}, {0.963, -0.191, 0.191}, {0.963, -0.191, 0.191}, {0.818, -0.181, 0.546}, {0.818, -0.181, 0.546}, {0.818, -0.181, 0.546}, {0.818, -0.546, 0.181}, {0.818, -0.546, 0.181}, {0.818, -0.546, 0.181}, {0.944, -0.234, 0.234}, {0.944, -0.234, 0.234}, {0.944, -0.234, 0.234}, {0.191, -0.963, 0.191}, {0.191, -0.963, 0.191}, {0.191, -0.963, 0.191}, {0.546, -0.818, 0.181}, {0.546, -0.818, 0.181}, {0.546, -0.818, 0.181}, {0.181, -0.818, 0.546}, {0.181, -0.818, 0.546}, {0.181, -0.818, 0.546}, {0.234, -0.944, 0.234}, {0.234, -0.944, 0.234}, {0.234, -0.944, 0.234}, {0.672, -0.672, 0.312}, {0.672, -0.672, 0.312}, {0.672, -0.672, 0.312}, {0.672, -0.312, 0.672}, {0.672, -0.312, 0.672}, {0.672, -0.312, 0.672}, {0.312, -0.672, 0.672}, {0.312, -0.672, 0.672}, {0.312, -0.672, 0.672}, {0.577, -0.577, 0.577}, {0.577, -0.577, 0.577}, {0.577, -0.577, 0.577}, {-0.191, -0.191, 0.963}, {-0.191, -0.191, 0.963}, {-0.191, -0.191, 0.963}, {-0.546, -0.181, 0.818}, {-0.546, -0.181, 0.818}, {-0.546, -0.181, 0.818}, {-0.181, -0.546, 0.818}, {-0.181, -0.546, 0.818}, {-0.181, -0.546, 0.818}, {-0.234, -0.234, 0.944}, {-0.234, -0.234, 0.944}, {-0.234, -0.234, 0.944}, {-0.963, -0.191, 0.191}, {-0.963, -0.191, 0.191}, {-0.963, -0.191, 0.191}, {-0.818, -0.181, 0.546}, {-0.818, -0.181, 0.546}, {-0.818, -0.181, 0.546}, {-0.818, -0.546, 0.181}, {-0.818, -0.546, 0.181}, {-0.818, -0.546, 0.181}, {-0.944, -0.234, 0.234}, {-0.944, -0.234, 0.234}, {-0.944, -0.234, 0.234}, {-0.191, -0.963, 0.191}, {-0.191, -0.963, 0.191}, {-0.191, -0.963, 0.191}, {-0.546, -0.818, 0.181}, {-0.546, -0.818, 0.181}, {-0.546, -0.818, 0.181}, {-0.181, -0.818, 0.546}, {-0.181, -0.818, 0.546}, {-0.181, -0.818, 0.546}, {-0.234, -0.944, 0.234}, {-0.234, -0.944, 0.234}, {-0.234, -0.944, 0.234}, {-0.672, -0.672, 0.312}, {-0.672, -0.672, 0.312}, {-0.672, -0.672, 0.312}, {-0.672, -0.312, 0.672}, {-0.672, -0.312, 0.672}, {-0.672, -0.312, 0.672}, {-0.312, -0.672, 0.672}, {-0.312, -0.672, 0.672}, {-0.312, -0.672, 0.672}, {-0.577, -0.577, 0.577}, {-0.577, -0.577, 0.577}, {-0.577, -0.577, 0.577}};

    std::vector<Vertex> data;
    data.reserve(points.size());
    assert(points.size() == normals.size());

    for (int i = 0; i < points.size(); i++) {
        data.push_back(Vertex{points[i], normals[i], {0.5f, 0.5f}, {-1, -1, -1, -1}, {0.f, 0.f, 0.f, 0.f}});
    }

    Mesh *newMesh = new Mesh(data);
    return newMesh;
}

Mesh *Mesh::GetCube() {
    std::vector<float> cubeVertices {
          // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    assert(cubeVertices.size() % 8 == 0);

    std::vector<Vertex> data;
    data.reserve(cubeVertices.size() / 8);

    for (int i = 0; i < cubeVertices.size(); i+=8) {
        Vec3 pos = {cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]};
        Vec3 norm = {cubeVertices[i + 3], cubeVertices[i + 4], cubeVertices[i + 5]};
        Vec2 tex = {cubeVertices[i + 6], cubeVertices[i + 7]};

        data.push_back(Vertex{pos, norm, tex, {-1, -1, -1, -1}, {0.f, 0.f, 0.f, 0.f}});
    }

    Mesh *newMesh = new Mesh(data);
    return newMesh;
}
