#ifndef SRC_MODEL_MODEL_H_
#define SRC_MODEL_MODEL_H_

#include <vector>

class Model {
 private:
    std::vector<float> points;
    std::vector<unsigned int> indices;

 public:
    // some variables
    unsigned int VAO, VBO, EBO;

    // constructor of class.
    explicit Model(const std::vector<float>& Points, const std::vector<unsigned int>& Indices);
    explicit Model(const std::vector<float>& Points);

    // deconstructor of class.
    ~Model();

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
#endif  // SRC_MODEL_MODEL_H_
