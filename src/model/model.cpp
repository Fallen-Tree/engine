#include "model.hpp"

#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdarg>

// constructor of class.
Model::Model(const std::vector<float>& Points, const std::vector<unsigned int>& Indices) {
    setPoints(Points);
    setIndices(Indices);
}

Model::Model(const std::vector<float>& Points, int vectorSize) {
    assert(vectorSize > 0);
    setPoints(Points);
    setIndices(vectorSize);
}

// getter and setter for working with class.
float* Model::getPoints() {
    assert(this->points.size() != 0);
    return this->points.data();
}

std::vector<float>& Model::getVecPoints() {
    return points;
}

unsigned int* Model::getIndices() {
    assert(indices.size() != 0);
    return this->indices.data();
}

std::vector<unsigned int>& Model::getVecIndices() {
    return indices;
}

void Model::setPoints(const std::vector<float>& points) {
    this->points.assign(points.begin(), points.end());
}

void Model::setIndices(const std::vector<unsigned int>& indices) {
    this->indices.assign(indices.begin(), indices.end());
}

// in this way we assume that the mathematical vector has size 3.
void Model::setIndices(int vectorSize) {
    assert(points.size() % vectorSize == 0);
    assert(vectorSize > 0);
    indices.resize(points.size() / vectorSize);
    for (unsigned int i = 0; i < points.size() / vectorSize; i++) {
        indices[i] = i;
    }
}

// get length of array.
int Model::getLenIndices() {
    return indices.size();
}

int Model::getLenArrPoints() {
    return points.size();
}

Model *Model::GetSphere() {
    std::vector<Vec3> points = {{0.000, 0.000, -1.000}, {0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {0.707, 0.000, -0.707}, {0.383, 0.000, -0.924}, {0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {0.408, 0.408, -0.816}, {0.000, 0.383, -0.924}, {0.408, 0.408, -0.816}, {0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, -0.383}, {0.924, 0.383, 0.000}, {0.707, 0.000, -0.707}, {0.924, 0.000, -0.383}, {0.816, 0.408, -0.408}, {0.707, 0.707, 0.000}, {0.816, 0.408, -0.408}, {0.924, 0.383, 0.000}, {0.816, 0.408, -0.408}, {0.924, 0.000, -0.383}, {0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {0.707, 0.707, 0.000}, {0.383, 0.924, 0.000}, {0.408, 0.816, -0.408}, {0.000, 0.707, -0.707}, {0.408, 0.816, -0.408}, {0.000, 0.924, -0.383}, {0.408, 0.816, -0.408}, {0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {0.707, 0.707, 0.000}, {0.816, 0.408, -0.408}, {0.408, 0.816, -0.408}, {0.707, 0.000, -0.707}, {0.816, 0.408, -0.408}, {0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {0.408, 0.408, -0.816}, {0.408, 0.816, -0.408}, {0.408, 0.408, -0.816}, {0.816, 0.408, -0.408}, {0.408, 0.816, -0.408}, {0.000, 0.000, -1.000}, {-0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {-0.707, 0.000, -0.707}, {-0.383, 0.000, -0.924}, {-0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {-0.408, 0.408, -0.816}, {0.000, 0.383, -0.924}, {-0.408, 0.408, -0.816}, {-0.383, 0.000, -0.924}, {0.000, 0.383, -0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, -0.383}, {-0.924, 0.383, 0.000}, {-0.707, 0.000, -0.707}, {-0.924, 0.000, -0.383}, {-0.816, 0.408, -0.408}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, -0.408}, {-0.924, 0.383, 0.000}, {-0.816, 0.408, -0.408}, {-0.924, 0.000, -0.383}, {-0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {-0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {-0.707, 0.707, 0.000}, {-0.383, 0.924, 0.000}, {-0.408, 0.816, -0.408}, {0.000, 0.707, -0.707}, {-0.408, 0.816, -0.408}, {0.000, 0.924, -0.383}, {-0.408, 0.816, -0.408}, {-0.383, 0.924, 0.000}, {0.000, 0.924, -0.383}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, -0.408}, {-0.408, 0.816, -0.408}, {-0.707, 0.000, -0.707}, {-0.816, 0.408, -0.408}, {-0.408, 0.408, -0.816}, {0.000, 0.707, -0.707}, {-0.408, 0.408, -0.816}, {-0.408, 0.816, -0.408}, {-0.408, 0.408, -0.816}, {-0.816, 0.408, -0.408}, {-0.408, 0.816, -0.408}, {0.000, 0.000, -1.000}, {0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {0.707, 0.000, -0.707}, {0.383, 0.000, -0.924}, {0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {0.408, -0.408, -0.816}, {0.000, -0.383, -0.924}, {0.408, -0.408, -0.816}, {0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, -0.383}, {0.924, -0.383, 0.000}, {0.707, 0.000, -0.707}, {0.924, 0.000, -0.383}, {0.816, -0.408, -0.408}, {0.707, -0.707, 0.000}, {0.816, -0.408, -0.408}, {0.924, -0.383, 0.000}, {0.816, -0.408, -0.408}, {0.924, 0.000, -0.383}, {0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {0.707, -0.707, 0.000}, {0.383, -0.924, 0.000}, {0.408, -0.816, -0.408}, {0.000, -0.707, -0.707}, {0.408, -0.816, -0.408}, {0.000, -0.924, -0.383}, {0.408, -0.816, -0.408}, {0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {0.707, -0.707, 0.000}, {0.816, -0.408, -0.408}, {0.408, -0.816, -0.408}, {0.707, 0.000, -0.707}, {0.816, -0.408, -0.408}, {0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {0.408, -0.408, -0.816}, {0.408, -0.816, -0.408}, {0.408, -0.408, -0.816}, {0.816, -0.408, -0.408}, {0.408, -0.816, -0.408}, {0.000, 0.000, -1.000}, {-0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {-0.707, 0.000, -0.707}, {-0.383, 0.000, -0.924}, {-0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {-0.408, -0.408, -0.816}, {0.000, -0.383, -0.924}, {-0.408, -0.408, -0.816}, {-0.383, 0.000, -0.924}, {0.000, -0.383, -0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, -0.383}, {-0.924, -0.383, 0.000}, {-0.707, 0.000, -0.707}, {-0.924, 0.000, -0.383}, {-0.816, -0.408, -0.408}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, -0.408}, {-0.924, -0.383, 0.000}, {-0.816, -0.408, -0.408}, {-0.924, 0.000, -0.383}, {-0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {-0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {-0.707, -0.707, 0.000}, {-0.383, -0.924, 0.000}, {-0.408, -0.816, -0.408}, {0.000, -0.707, -0.707}, {-0.408, -0.816, -0.408}, {0.000, -0.924, -0.383}, {-0.408, -0.816, -0.408}, {-0.383, -0.924, 0.000}, {0.000, -0.924, -0.383}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, -0.408}, {-0.408, -0.816, -0.408}, {-0.707, 0.000, -0.707}, {-0.816, -0.408, -0.408}, {-0.408, -0.408, -0.816}, {0.000, -0.707, -0.707}, {-0.408, -0.408, -0.816}, {-0.408, -0.816, -0.408}, {-0.408, -0.408, -0.816}, {-0.816, -0.408, -0.408}, {-0.408, -0.816, -0.408}, {0.000, 0.000, 1.000}, {0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {0.707, 0.000, 0.707}, {0.383, 0.000, 0.924}, {0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {0.408, 0.408, 0.816}, {0.000, 0.383, 0.924}, {0.408, 0.408, 0.816}, {0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, 0.383}, {0.924, 0.383, 0.000}, {0.707, 0.000, 0.707}, {0.924, 0.000, 0.383}, {0.816, 0.408, 0.408}, {0.707, 0.707, 0.000}, {0.816, 0.408, 0.408}, {0.924, 0.383, 0.000}, {0.816, 0.408, 0.408}, {0.924, 0.000, 0.383}, {0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {0.707, 0.707, 0.000}, {0.383, 0.924, 0.000}, {0.408, 0.816, 0.408}, {0.000, 0.707, 0.707}, {0.408, 0.816, 0.408}, {0.000, 0.924, 0.383}, {0.408, 0.816, 0.408}, {0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {0.707, 0.707, 0.000}, {0.816, 0.408, 0.408}, {0.408, 0.816, 0.408}, {0.707, 0.000, 0.707}, {0.816, 0.408, 0.408}, {0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {0.408, 0.408, 0.816}, {0.408, 0.816, 0.408}, {0.408, 0.408, 0.816}, {0.816, 0.408, 0.408}, {0.408, 0.816, 0.408}, {0.000, 0.000, 1.000}, {-0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {-0.707, 0.000, 0.707}, {-0.383, 0.000, 0.924}, {-0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {-0.408, 0.408, 0.816}, {0.000, 0.383, 0.924}, {-0.408, 0.408, 0.816}, {-0.383, 0.000, 0.924}, {0.000, 0.383, 0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, 0.383}, {-0.924, 0.383, 0.000}, {-0.707, 0.000, 0.707}, {-0.924, 0.000, 0.383}, {-0.816, 0.408, 0.408}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, 0.408}, {-0.924, 0.383, 0.000}, {-0.816, 0.408, 0.408}, {-0.924, 0.000, 0.383}, {-0.924, 0.383, 0.000}, {0.000, 1.000, 0.000}, {-0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {-0.707, 0.707, 0.000}, {-0.383, 0.924, 0.000}, {-0.408, 0.816, 0.408}, {0.000, 0.707, 0.707}, {-0.408, 0.816, 0.408}, {0.000, 0.924, 0.383}, {-0.408, 0.816, 0.408}, {-0.383, 0.924, 0.000}, {0.000, 0.924, 0.383}, {-0.707, 0.707, 0.000}, {-0.816, 0.408, 0.408}, {-0.408, 0.816, 0.408}, {-0.707, 0.000, 0.707}, {-0.816, 0.408, 0.408}, {-0.408, 0.408, 0.816}, {0.000, 0.707, 0.707}, {-0.408, 0.408, 0.816}, {-0.408, 0.816, 0.408}, {-0.408, 0.408, 0.816}, {-0.816, 0.408, 0.408}, {-0.408, 0.816, 0.408}, {0.000, 0.000, 1.000}, {0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {0.707, 0.000, 0.707}, {0.383, 0.000, 0.924}, {0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {0.408, -0.408, 0.816}, {0.000, -0.383, 0.924}, {0.408, -0.408, 0.816}, {0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {1.000, 0.000, 0.000}, {0.924, 0.000, 0.383}, {0.924, -0.383, 0.000}, {0.707, 0.000, 0.707}, {0.924, 0.000, 0.383}, {0.816, -0.408, 0.408}, {0.707, -0.707, 0.000}, {0.816, -0.408, 0.408}, {0.924, -0.383, 0.000}, {0.816, -0.408, 0.408}, {0.924, 0.000, 0.383}, {0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {0.707, -0.707, 0.000}, {0.383, -0.924, 0.000}, {0.408, -0.816, 0.408}, {0.000, -0.707, 0.707}, {0.408, -0.816, 0.408}, {0.000, -0.924, 0.383}, {0.408, -0.816, 0.408}, {0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {0.707, -0.707, 0.000}, {0.816, -0.408, 0.408}, {0.408, -0.816, 0.408}, {0.707, 0.000, 0.707}, {0.816, -0.408, 0.408}, {0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {0.408, -0.408, 0.816}, {0.408, -0.816, 0.408}, {0.408, -0.408, 0.816}, {0.816, -0.408, 0.408}, {0.408, -0.816, 0.408}, {0.000, 0.000, 1.000}, {-0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {-0.707, 0.000, 0.707}, {-0.383, 0.000, 0.924}, {-0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {-0.408, -0.408, 0.816}, {0.000, -0.383, 0.924}, {-0.408, -0.408, 0.816}, {-0.383, 0.000, 0.924}, {0.000, -0.383, 0.924}, {-1.000, 0.000, 0.000}, {-0.924, 0.000, 0.383}, {-0.924, -0.383, 0.000}, {-0.707, 0.000, 0.707}, {-0.924, 0.000, 0.383}, {-0.816, -0.408, 0.408}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, 0.408}, {-0.924, -0.383, 0.000}, {-0.816, -0.408, 0.408}, {-0.924, 0.000, 0.383}, {-0.924, -0.383, 0.000}, {0.000, -1.000, 0.000}, {-0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {-0.707, -0.707, 0.000}, {-0.383, -0.924, 0.000}, {-0.408, -0.816, 0.408}, {0.000, -0.707, 0.707}, {-0.408, -0.816, 0.408}, {0.000, -0.924, 0.383}, {-0.408, -0.816, 0.408}, {-0.383, -0.924, 0.000}, {0.000, -0.924, 0.383}, {-0.707, -0.707, 0.000}, {-0.816, -0.408, 0.408}, {-0.408, -0.816, 0.408}, {-0.707, 0.000, 0.707}, {-0.816, -0.408, 0.408}, {-0.408, -0.408, 0.816}, {0.000, -0.707, 0.707}, {-0.408, -0.408, 0.816}, {-0.408, -0.816, 0.408}, {-0.408, -0.408, 0.816}, {-0.816, -0.408, 0.408}, {-0.408, -0.816, 0.408}};
    std::vector<Vec3> normals = {{0.191, 0.191, -0.963}, {0.191, 0.191, -0.963}, {0.191, 0.191, -0.963}, {0.546, 0.181, -0.818}, {0.546, 0.181, -0.818}, {0.546, 0.181, -0.818}, {0.181, 0.546, -0.818}, {0.181, 0.546, -0.818}, {0.181, 0.546, -0.818}, {0.234, 0.234, -0.944}, {0.234, 0.234, -0.944}, {0.234, 0.234, -0.944}, {0.963, 0.191, -0.191}, {0.963, 0.191, -0.191}, {0.963, 0.191, -0.191}, {0.818, 0.181, -0.546}, {0.818, 0.181, -0.546}, {0.818, 0.181, -0.546}, {0.818, 0.546, -0.181}, {0.818, 0.546, -0.181}, {0.818, 0.546, -0.181}, {0.944, 0.234, -0.234}, {0.944, 0.234, -0.234}, {0.944, 0.234, -0.234}, {0.191, 0.963, -0.191}, {0.191, 0.963, -0.191}, {0.191, 0.963, -0.191}, {0.546, 0.818, -0.181}, {0.546, 0.818, -0.181}, {0.546, 0.818, -0.181}, {0.181, 0.818, -0.546}, {0.181, 0.818, -0.546}, {0.181, 0.818, -0.546}, {0.234, 0.944, -0.234}, {0.234, 0.944, -0.234}, {0.234, 0.944, -0.234}, {0.672, 0.672, -0.312}, {0.672, 0.672, -0.312}, {0.672, 0.672, -0.312}, {0.672, 0.312, -0.672}, {0.672, 0.312, -0.672}, {0.672, 0.312, -0.672}, {0.312, 0.672, -0.672}, {0.312, 0.672, -0.672}, {0.312, 0.672, -0.672}, {0.577, 0.577, -0.577}, {0.577, 0.577, -0.577}, {0.577, 0.577, -0.577}, {-0.191, 0.191, -0.963}, {-0.191, 0.191, -0.963}, {-0.191, 0.191, -0.963}, {-0.546, 0.181, -0.818}, {-0.546, 0.181, -0.818}, {-0.546, 0.181, -0.818}, {-0.181, 0.546, -0.818}, {-0.181, 0.546, -0.818}, {-0.181, 0.546, -0.818}, {-0.234, 0.234, -0.944}, {-0.234, 0.234, -0.944}, {-0.234, 0.234, -0.944}, {-0.963, 0.191, -0.191}, {-0.963, 0.191, -0.191}, {-0.963, 0.191, -0.191}, {-0.818, 0.181, -0.546}, {-0.818, 0.181, -0.546}, {-0.818, 0.181, -0.546}, {-0.818, 0.546, -0.181}, {-0.818, 0.546, -0.181}, {-0.818, 0.546, -0.181}, {-0.944, 0.234, -0.234}, {-0.944, 0.234, -0.234}, {-0.944, 0.234, -0.234}, {-0.191, 0.963, -0.191}, {-0.191, 0.963, -0.191}, {-0.191, 0.963, -0.191}, {-0.546, 0.818, -0.181}, {-0.546, 0.818, -0.181}, {-0.546, 0.818, -0.181}, {-0.181, 0.818, -0.546}, {-0.181, 0.818, -0.546}, {-0.181, 0.818, -0.546}, {-0.234, 0.944, -0.234}, {-0.234, 0.944, -0.234}, {-0.234, 0.944, -0.234}, {-0.672, 0.672, -0.312}, {-0.672, 0.672, -0.312}, {-0.672, 0.672, -0.312}, {-0.672, 0.312, -0.672}, {-0.672, 0.312, -0.672}, {-0.672, 0.312, -0.672}, {-0.312, 0.672, -0.672}, {-0.312, 0.672, -0.672}, {-0.312, 0.672, -0.672}, {-0.577, 0.577, -0.577}, {-0.577, 0.577, -0.577}, {-0.577, 0.577, -0.577}, {0.191, -0.191, -0.963}, {0.191, -0.191, -0.963}, {0.191, -0.191, -0.963}, {0.546, -0.181, -0.818}, {0.546, -0.181, -0.818}, {0.546, -0.181, -0.818}, {0.181, -0.546, -0.818}, {0.181, -0.546, -0.818}, {0.181, -0.546, -0.818}, {0.234, -0.234, -0.944}, {0.234, -0.234, -0.944}, {0.234, -0.234, -0.944}, {0.963, -0.191, -0.191}, {0.963, -0.191, -0.191}, {0.963, -0.191, -0.191}, {0.818, -0.181, -0.546}, {0.818, -0.181, -0.546}, {0.818, -0.181, -0.546}, {0.818, -0.546, -0.181}, {0.818, -0.546, -0.181}, {0.818, -0.546, -0.181}, {0.944, -0.234, -0.234}, {0.944, -0.234, -0.234}, {0.944, -0.234, -0.234}, {0.191, -0.963, -0.191}, {0.191, -0.963, -0.191}, {0.191, -0.963, -0.191}, {0.546, -0.818, -0.181}, {0.546, -0.818, -0.181}, {0.546, -0.818, -0.181}, {0.181, -0.818, -0.546}, {0.181, -0.818, -0.546}, {0.181, -0.818, -0.546}, {0.234, -0.944, -0.234}, {0.234, -0.944, -0.234}, {0.234, -0.944, -0.234}, {0.672, -0.672, -0.312}, {0.672, -0.672, -0.312}, {0.672, -0.672, -0.312}, {0.672, -0.312, -0.672}, {0.672, -0.312, -0.672}, {0.672, -0.312, -0.672}, {0.312, -0.672, -0.672}, {0.312, -0.672, -0.672}, {0.312, -0.672, -0.672}, {0.577, -0.577, -0.577}, {0.577, -0.577, -0.577}, {0.577, -0.577, -0.577}, {-0.191, -0.191, -0.963}, {-0.191, -0.191, -0.963}, {-0.191, -0.191, -0.963}, {-0.546, -0.181, -0.818}, {-0.546, -0.181, -0.818}, {-0.546, -0.181, -0.818}, {-0.181, -0.546, -0.818}, {-0.181, -0.546, -0.818}, {-0.181, -0.546, -0.818}, {-0.234, -0.234, -0.944}, {-0.234, -0.234, -0.944}, {-0.234, -0.234, -0.944}, {-0.963, -0.191, -0.191}, {-0.963, -0.191, -0.191}, {-0.963, -0.191, -0.191}, {-0.818, -0.181, -0.546}, {-0.818, -0.181, -0.546}, {-0.818, -0.181, -0.546}, {-0.818, -0.546, -0.181}, {-0.818, -0.546, -0.181}, {-0.818, -0.546, -0.181}, {-0.944, -0.234, -0.234}, {-0.944, -0.234, -0.234}, {-0.944, -0.234, -0.234}, {-0.191, -0.963, -0.191}, {-0.191, -0.963, -0.191}, {-0.191, -0.963, -0.191}, {-0.546, -0.818, -0.181}, {-0.546, -0.818, -0.181}, {-0.546, -0.818, -0.181}, {-0.181, -0.818, -0.546}, {-0.181, -0.818, -0.546}, {-0.181, -0.818, -0.546}, {-0.234, -0.944, -0.234}, {-0.234, -0.944, -0.234}, {-0.234, -0.944, -0.234}, {-0.672, -0.672, -0.312}, {-0.672, -0.672, -0.312}, {-0.672, -0.672, -0.312}, {-0.672, -0.312, -0.672}, {-0.672, -0.312, -0.672}, {-0.672, -0.312, -0.672}, {-0.312, -0.672, -0.672}, {-0.312, -0.672, -0.672}, {-0.312, -0.672, -0.672}, {-0.577, -0.577, -0.577}, {-0.577, -0.577, -0.577}, {-0.577, -0.577, -0.577}, {0.191, 0.191, 0.963}, {0.191, 0.191, 0.963}, {0.191, 0.191, 0.963}, {0.546, 0.181, 0.818}, {0.546, 0.181, 0.818}, {0.546, 0.181, 0.818}, {0.181, 0.546, 0.818}, {0.181, 0.546, 0.818}, {0.181, 0.546, 0.818}, {0.234, 0.234, 0.944}, {0.234, 0.234, 0.944}, {0.234, 0.234, 0.944}, {0.963, 0.191, 0.191}, {0.963, 0.191, 0.191}, {0.963, 0.191, 0.191}, {0.818, 0.181, 0.546}, {0.818, 0.181, 0.546}, {0.818, 0.181, 0.546}, {0.818, 0.546, 0.181}, {0.818, 0.546, 0.181}, {0.818, 0.546, 0.181}, {0.944, 0.234, 0.234}, {0.944, 0.234, 0.234}, {0.944, 0.234, 0.234}, {0.191, 0.963, 0.191}, {0.191, 0.963, 0.191}, {0.191, 0.963, 0.191}, {0.546, 0.818, 0.181}, {0.546, 0.818, 0.181}, {0.546, 0.818, 0.181}, {0.181, 0.818, 0.546}, {0.181, 0.818, 0.546}, {0.181, 0.818, 0.546}, {0.234, 0.944, 0.234}, {0.234, 0.944, 0.234}, {0.234, 0.944, 0.234}, {0.672, 0.672, 0.312}, {0.672, 0.672, 0.312}, {0.672, 0.672, 0.312}, {0.672, 0.312, 0.672}, {0.672, 0.312, 0.672}, {0.672, 0.312, 0.672}, {0.312, 0.672, 0.672}, {0.312, 0.672, 0.672}, {0.312, 0.672, 0.672}, {0.577, 0.577, 0.577}, {0.577, 0.577, 0.577}, {0.577, 0.577, 0.577}, {-0.191, 0.191, 0.963}, {-0.191, 0.191, 0.963}, {-0.191, 0.191, 0.963}, {-0.546, 0.181, 0.818}, {-0.546, 0.181, 0.818}, {-0.546, 0.181, 0.818}, {-0.181, 0.546, 0.818}, {-0.181, 0.546, 0.818}, {-0.181, 0.546, 0.818}, {-0.234, 0.234, 0.944}, {-0.234, 0.234, 0.944}, {-0.234, 0.234, 0.944}, {-0.963, 0.191, 0.191}, {-0.963, 0.191, 0.191}, {-0.963, 0.191, 0.191}, {-0.818, 0.181, 0.546}, {-0.818, 0.181, 0.546}, {-0.818, 0.181, 0.546}, {-0.818, 0.546, 0.181}, {-0.818, 0.546, 0.181}, {-0.818, 0.546, 0.181}, {-0.944, 0.234, 0.234}, {-0.944, 0.234, 0.234}, {-0.944, 0.234, 0.234}, {-0.191, 0.963, 0.191}, {-0.191, 0.963, 0.191}, {-0.191, 0.963, 0.191}, {-0.546, 0.818, 0.181}, {-0.546, 0.818, 0.181}, {-0.546, 0.818, 0.181}, {-0.181, 0.818, 0.546}, {-0.181, 0.818, 0.546}, {-0.181, 0.818, 0.546}, {-0.234, 0.944, 0.234}, {-0.234, 0.944, 0.234}, {-0.234, 0.944, 0.234}, {-0.672, 0.672, 0.312}, {-0.672, 0.672, 0.312}, {-0.672, 0.672, 0.312}, {-0.672, 0.312, 0.672}, {-0.672, 0.312, 0.672}, {-0.672, 0.312, 0.672}, {-0.312, 0.672, 0.672}, {-0.312, 0.672, 0.672}, {-0.312, 0.672, 0.672}, {-0.577, 0.577, 0.577}, {-0.577, 0.577, 0.577}, {-0.577, 0.577, 0.577}, {0.191, -0.191, 0.963}, {0.191, -0.191, 0.963}, {0.191, -0.191, 0.963}, {0.546, -0.181, 0.818}, {0.546, -0.181, 0.818}, {0.546, -0.181, 0.818}, {0.181, -0.546, 0.818}, {0.181, -0.546, 0.818}, {0.181, -0.546, 0.818}, {0.234, -0.234, 0.944}, {0.234, -0.234, 0.944}, {0.234, -0.234, 0.944}, {0.963, -0.191, 0.191}, {0.963, -0.191, 0.191}, {0.963, -0.191, 0.191}, {0.818, -0.181, 0.546}, {0.818, -0.181, 0.546}, {0.818, -0.181, 0.546}, {0.818, -0.546, 0.181}, {0.818, -0.546, 0.181}, {0.818, -0.546, 0.181}, {0.944, -0.234, 0.234}, {0.944, -0.234, 0.234}, {0.944, -0.234, 0.234}, {0.191, -0.963, 0.191}, {0.191, -0.963, 0.191}, {0.191, -0.963, 0.191}, {0.546, -0.818, 0.181}, {0.546, -0.818, 0.181}, {0.546, -0.818, 0.181}, {0.181, -0.818, 0.546}, {0.181, -0.818, 0.546}, {0.181, -0.818, 0.546}, {0.234, -0.944, 0.234}, {0.234, -0.944, 0.234}, {0.234, -0.944, 0.234}, {0.672, -0.672, 0.312}, {0.672, -0.672, 0.312}, {0.672, -0.672, 0.312}, {0.672, -0.312, 0.672}, {0.672, -0.312, 0.672}, {0.672, -0.312, 0.672}, {0.312, -0.672, 0.672}, {0.312, -0.672, 0.672}, {0.312, -0.672, 0.672}, {0.577, -0.577, 0.577}, {0.577, -0.577, 0.577}, {0.577, -0.577, 0.577}, {-0.191, -0.191, 0.963}, {-0.191, -0.191, 0.963}, {-0.191, -0.191, 0.963}, {-0.546, -0.181, 0.818}, {-0.546, -0.181, 0.818}, {-0.546, -0.181, 0.818}, {-0.181, -0.546, 0.818}, {-0.181, -0.546, 0.818}, {-0.181, -0.546, 0.818}, {-0.234, -0.234, 0.944}, {-0.234, -0.234, 0.944}, {-0.234, -0.234, 0.944}, {-0.963, -0.191, 0.191}, {-0.963, -0.191, 0.191}, {-0.963, -0.191, 0.191}, {-0.818, -0.181, 0.546}, {-0.818, -0.181, 0.546}, {-0.818, -0.181, 0.546}, {-0.818, -0.546, 0.181}, {-0.818, -0.546, 0.181}, {-0.818, -0.546, 0.181}, {-0.944, -0.234, 0.234}, {-0.944, -0.234, 0.234}, {-0.944, -0.234, 0.234}, {-0.191, -0.963, 0.191}, {-0.191, -0.963, 0.191}, {-0.191, -0.963, 0.191}, {-0.546, -0.818, 0.181}, {-0.546, -0.818, 0.181}, {-0.546, -0.818, 0.181}, {-0.181, -0.818, 0.546}, {-0.181, -0.818, 0.546}, {-0.181, -0.818, 0.546}, {-0.234, -0.944, 0.234}, {-0.234, -0.944, 0.234}, {-0.234, -0.944, 0.234}, {-0.672, -0.672, 0.312}, {-0.672, -0.672, 0.312}, {-0.672, -0.672, 0.312}, {-0.672, -0.312, 0.672}, {-0.672, -0.312, 0.672}, {-0.672, -0.312, 0.672}, {-0.312, -0.672, 0.672}, {-0.312, -0.672, 0.672}, {-0.312, -0.672, 0.672}, {-0.577, -0.577, 0.577}, {-0.577, -0.577, 0.577}, {-0.577, -0.577, 0.577}};

    std::vector<float> data;
    data.reserve(points.size() * 8);
    assert(points.size() == normals.size());

    for (int i = 0; i < points.size(); i++) {
        data.push_back(points[i].x);
        data.push_back(points[i].y);
        data.push_back(points[i].z);

        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);

        // For uvs
        data.push_back(0.5);
        data.push_back(0.5);
    }
    return new Model(data, 8);
}
