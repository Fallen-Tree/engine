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

// in this way we assume that the mathematical vector has size 3.
Model::Model(const std::vector<float>& Points) {
    assert(Points.size() % 3 == 0);
    setPoints(Points);
    setIndices();
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
void Model::setIndices() {
    assert(points.size() % 3 == 0);
    for (unsigned int i = 0; i < points.size() / 3; i++) {
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

Model::Model(const char* path) {
    std::ifstream objFile;
    // ensure ifstream objects can throw exceptions:
    objFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        objFile.open(path);
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    std::vector<float> modelPoints(0);
    std::vector<unsigned> modelIndices(0);
    if (objFile.is_open()) {
        while (objFile.good()) {
            std::string line;
            std::getline(objFile, line);
            std::stringstream lineStream;
            lineStream << line;
            std::string key;
            if (!(lineStream >> key)) continue;
            // std::cout << line << std::endl;
            if (key == "v") {
                // read vertice coordinates
                float x, y, z;
                lineStream >> x >> y >> z;
                modelPoints.push_back(x / 20);
                modelPoints.push_back(y / 20);
                modelPoints.push_back(z / 20);
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
                // not a valid key
            }
        }
    }
    setPoints(modelPoints);
    setIndices(modelIndices);
}
