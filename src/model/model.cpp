#include "model.h"

#include <vector>
#include <cassert>
#include <algorithm>


// constructor of class.
model::model(const std::vector<float>& arrayOfPoints, const std::vector<unsigned int>& arrayOfIndices) {
    this->arrayOfPoints = new float[arrayOfPoints.size()];
    this->arrayOfIndices = new unsigned int[arrayOfIndices.size()];
    lengthOfArrayOfIndices = arrayOfIndices.size();
    lengthOfArrayOfPoints = arrayOfPoints.size();

    copy(arrayOfPoints.begin(), arrayOfPoints.end(), this->arrayOfPoints);
    copy(arrayOfIndices.begin(), arrayOfIndices.end(), this->arrayOfIndices);
}

model::model(const std::vector<float>& arrayOfPoints) {
    this->arrayOfPoints = new float[arrayOfPoints.size()];
    this->arrayOfIndices = NULL;
    lengthOfArrayOfPoints = arrayOfPoints.size();
    lengthOfArrayOfIndices = 0;

    copy(arrayOfPoints.begin(), arrayOfPoints.end(), this->arrayOfPoints);
}

model::model() {
    this->arrayOfPoints = NULL;
    this->arrayOfIndices = NULL;
    lengthOfArrayOfIndices = 0;
    lengthOfArrayOfPoints = 0;
}

// deconstructor of class.
model::~model() {
    if (this->arrayOfPoints) {
        delete[] this->arrayOfPoints;
    }
    if (this->arrayOfIndices) {
        delete[] this->arrayOfIndices;
    }
}

// getter and setter for working with class.
float* model::getArrayOfPoints() {
    assert(this->arrayOfPoints != NULL);
    return this->arrayOfPoints;
}

unsigned int* model::getArrayOfIndices() {
    assert(this->arrayOfIndices != NULL);
    return this->arrayOfIndices;
}

void model::setArrayOfPoints(const std::vector<float>& arrayOfPoints) {
    // free memory.
    if (this->arrayOfPoints != NULL) {
        delete[] this->arrayOfPoints;
    }

    this->arrayOfPoints = new float[arrayOfPoints.size()];
    lengthOfArrayOfPoints = arrayOfPoints.size();

    copy(arrayOfPoints.begin(), arrayOfPoints.end(), this->arrayOfPoints);
}

void model::setArrayOfIndices(const std::vector<int>& arrayOfIndices) {
    // free memory.
    if (this->arrayOfIndices != NULL) {
        delete[] this->arrayOfIndices;
    }
    this->arrayOfIndices = new unsigned int[arrayOfIndices.size()];
    lengthOfArrayOfIndices = arrayOfIndices.size();

    copy(arrayOfIndices.begin(), arrayOfIndices.end(), this->arrayOfIndices);
}

// get length of array.
int model::getLengthOfArrayOfIndices() {
    return lengthOfArrayOfIndices;
}

int model::getLengthofArrayOfPoints() {
    return lengthOfArrayOfPoints;
}
