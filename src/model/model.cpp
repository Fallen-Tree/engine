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

Model* Model::loadFromFile(const char* filePath) {
    std::ifstream objFile;
    // ensure ifstream objects can throw exceptions:
    objFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        objFile.open(filePath);
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::MODEL::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    std::vector<float> modelPoints(0);
    std::vector<unsigned> modelIndices(0);
    if (objFile.is_open()) {
        int line_num = 0;
        std::string line;
        while (objFile.good()) {
            std::getline(objFile, line);
            std::stringstream lineStream;
            lineStream << line;
            std::string key;
            lineStream >> key;
            if (key == "v") {
                // read vertice coordinates
                float x, y, z;
                lineStream >> x >> y >> z;
                modelPoints.push_back(x);
                modelPoints.push_back(y);
                modelPoints.push_back(z);
                modelPoints.push_back(1);
                modelPoints.push_back(1);
                modelPoints.push_back(1);
            } else if (key == "f") {
                // add triangles
                std::vector<unsigned> faceIndices;
                std::string verticeString;
                while (lineStream >> verticeString) {
                    std::string indexString = "";
                    for (unsigned i = 0; i < verticeString.size(); ++i) {
                        if (verticeString[i] == '/') {
                            break;
                        } else {
                            indexString += verticeString[i];
                        }
                    }
                    faceIndices.push_back(std::stoi(indexString) - 1);
                }
                for (int i = 1; i < faceIndices.size() - 1; ++i) {
                    modelIndices.push_back(faceIndices[0]);
                    modelIndices.push_back(faceIndices[i]);
                    modelIndices.push_back(faceIndices[i + 1]);
                }
            } else {
                // not a valid key at the start of the string
            }
        }
        objFile.close();
    }
    Model* newModel = new Model(modelPoints, modelIndices);
    return newModel;
}
