#pragma once

#include <vector>
#include <memory>
#include "shaders.hpp"

class Model {
 private:
    std::vector<float> points;
    std::vector<unsigned int> indices;
    std::shared_ptr<ShaderProgram> shader = std::make_shared<ShaderProgram>(ShaderProgram());

 public:
    // some variables
    // constructor of class.
    explicit Model(const std::vector<float>& Points, const std::vector<unsigned int>& Indices);

    explicit Model(const std::vector<float>& Points, int vectorSize);

    // getter and setter for working with class.
    float* getPoints();

    std::vector<float>& getVecPoints();

    unsigned int* getIndices();

    std::vector<unsigned int>& getVecIndices();

    void setPoints(const std::vector<float>& points);

    void setIndices(const std::vector<unsigned int>& indices);

    void setIndices(int vectorSize);

    // get length of array.
    int getLenIndices();

    int getLenArrPoints();

    ShaderProgram * getShader();

    void setShader(const ShaderProgram &sp);
};
