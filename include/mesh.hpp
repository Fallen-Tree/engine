#pragma once

#include <vector>
#include "transform.hpp"

class Mesh {
 private:
    std::vector<float> points;
    std::vector<unsigned int> indices;

 public:

    Mesh() {}

    Mesh(const std::vector<float>& Points, const std::vector<unsigned int>& Indices);

    Mesh(const std::vector<float>& Points, int vectorSize);

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

    Mesh *GetSphere();

    Mesh Transformed(Transform tranform);

    Vec3 ClosestPoint(Vec3 point, Transform transform);
    Vec3 CollisionNormal(Vec3 point, Transform tranform);
};
