#pragma once

#include <vector>
#include "transform.hpp"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh {
 private:
    std::vector<Vertex> points;
    std::vector<unsigned int> indices;

 public:
    /* Constructors */
    Mesh() {}
    Mesh(const std::vector<Vertex>& Points, const std::vector<unsigned int>& Indices);
    explicit Mesh(const std::vector<Vertex>& Points);

    /* Getters */
    Vertex* getPoints();
    std::vector<Vertex>& getVecPoints();

    unsigned int* getIndices();
    std::vector<unsigned int>& getVecIndices();

    int getLenIndices();
    int getLenArrPoints();

    static Mesh *GetSphere();
    static Mesh *GetCube();

    /* Setters */
    void setPoints(const std::vector<Vertex>& points);
    void setIndices(const std::vector<unsigned int>& indices);
    void setDefaultIndices();

    // Mesh Transformed(Transform tranform);

     Vec3 ClosestPoint(Vec3 point, Transform transform);
     Vec3 CollisionNormal(Vec3 point, Transform tranform);
};
