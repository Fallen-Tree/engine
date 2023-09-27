#ifndef SRC_MODEL_MODEL_H_
#define SRC_MODEL_MODEL_H_

#include <vector>

class Model {
 private:
    float* Points;
    unsigned int * Indices;
    unsigned int lenPoints, lenIndices;

 public:
    // some variables
    unsigned int VAO, VBO, EBO;

    // constructor of class.
    Model(const std::vector<float>& Points, const std::vector<unsigned int>& Indices);
    Model(const std::vector<float>& Points);
    Model();

    // deconstructor of class.
    ~Model();

    // getter and setter for working with class.
    float* getPoints();

    unsigned int* getIndices();

    void setPoints(const std::vector<float>& Points);

    void setIndices(const std::vector<unsigned int>& Indices);

    void setIndices();

    // get length of array.
    int getLenIndices();

    int getLenPoints();
};
#endif  // SRC_MODEL_MODEL_H_
