#include "model.hpp"

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
