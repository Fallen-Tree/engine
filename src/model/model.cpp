#include "model.h"

#include <vector>
#include <cassert>
#include <algorithm>


// constructor of class.
Model::Model(const std::vector<float>& Points, const std::vector<unsigned int>& Indices) {
    this->Points = NULL;
    this->Indices = NULL;
    setPoints(Points);
    setIndices(Indices);
}

// in this way we assume that the mathematical vector has size 3.
Model::Model(const std::vector<float>& Points) {
    this->Points = NULL;
    this->Indices = NULL;
    setPoints(Points);
    setIndices();
}

Model::Model() {
    this->Points = NULL;
    this->Indices = NULL;
    lenIndices = 0;
    lenPoints = 0;
}

// deconstructor of class.
Model::~Model() {
    if (this->Points) {
        delete[] this->Points;
    }
    if (this->Indices) {
        delete[] this->Indices;
    }
}

// getter and setter for working with class.
float* Model::getPoints() {
    assert(this->Points != NULL);
    return this->Points;
}

unsigned int* Model::getIndices() {
    assert(this->Indices != NULL);
    return this->Indices;
}

void Model::setPoints(const std::vector<float>& Points) {
    if (this->Points != NULL) {
        // if we have array with equal len, we just copy from Points.
        if (Points.size() ==  this->lenPoints) {
            copy(Points.begin(), Points.end(), this->Points);
            return;
        }

        // free memory.
        delete[] this->Points;
    }

    this->Points = new float[Points.size()];
    lenPoints = Points.size();
    copy(Points.begin(), Points.end(), this->Points);
}

void Model::setIndices(const std::vector<unsigned int>& Indices) {
    if (this->Indices != NULL) {
        // if we have array with equal len, we just copy from Indices.
        if (Indices.size() == lenIndices) {
            copy(Indices.begin(), Indices.end(), this->Indices);
            return;
        }

        // free memory.
        delete[] this->Indices;
    }

    this->Indices = new unsigned int[Indices.size()];
    lenIndices = Indices.size();
    copy(Indices.begin(), Indices.end(), this->Indices);
}

// in this way we assume that the mathematical vector has size 3.
void Model::setIndices() {
    if (this->Indices != NULL) {
        if (lenPoints / 3 == lenIndices) {
            for (unsigned int i = 0; i < lenPoints; i++) {
                this->Indices[i] = i;
            }
            return;
        }

    // free memory.
    delete[] this->Indices;
    }

    lenIndices = lenPoints / 3;
    this->Indices = new unsigned int[lenIndices];

    for (unsigned int i = 0; i < lenIndices; i++) {
        this->Indices[i] = i;
    }
}

// get length of array.
int Model::getLenIndices() {
    return lenIndices;
}

int Model::getLenPoints() {
    return lenPoints;
}
