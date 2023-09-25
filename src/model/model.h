#ifndef SRC_MODEL_MODEL_H_
#define MODEL_H

#include <vector>

class model {
 private:
    float* arrayOfPoints;
    unsigned int * arrayOfIndices;
    int lengthOfArrayOfPoints, lengthOfArrayOfIndices;

 public:
    // some variables
    unsigned int VAO, VBO, EBO;

    // constructor of class.
    model(const std::vector<float>& arrayOfPoints, const std::vector<unsigned int>& arrayOfIndices);
    model(const std::vector<float>& arrayOfPoints);
    model();

    // deconstructor of class.
    ~model();

    // getter and setter for working with class.
    float* getArrayOfPoints();

    unsigned int* getArrayOfIndices();

    void setArrayOfPoints(const std::vector<float>& arrayOfPoints);

    void setArrayOfIndices(const std::vector<int>& arrayOfIndices);

    // get length of array.
    int getLengthOfArrayOfIndices();

    int getLengthofArrayOfPoints();
};
#endif
