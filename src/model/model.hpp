#ifndef SRC_MODEL_MODEL_HPP_
#define SRC_MODEL_MODEL_HPP_

#include <vector>
#include "shader_loader.hpp"

class Model {
 private:
    std::vector<float> points;
    std::vector<unsigned int> indices;

 public:
    // some variables
    unsigned int VAO, VBO, EBO;
    ShaderProgram shader = ShaderProgram();

    // constructor of class.
    explicit Model(const std::vector<float>& Points, const std::vector<unsigned int>& Indices);

    explicit Model(const std::vector<float>& Points);

    explicit Model(const char* path);

    // getter and setter for working with class.
    float* getPoints();

    std::vector<float>& getVecPoints();

    unsigned int* getIndices();

    std::vector<unsigned int>& getVecIndices();

    void setPoints(const std::vector<float>& points);

    void setIndices(const std::vector<unsigned int>& indices);

    void setIndices();

    // get length of array.
    int getLenIndices();

    int getLenArrPoints();
};
#endif  // SRC_MODEL_MODEL_HPP_
