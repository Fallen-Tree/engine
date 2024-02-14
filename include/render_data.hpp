#pragma once

#include<vector>

#include "material.hpp"
#include "mesh.hpp"

class RenderMesh : public Mesh {
 public:
    Material material;
    unsigned int VAO, VBO, EBO;

    RenderMesh(std::vector<float> points, std::vector<unsigned int> indices, Material material);
};

void bindRenderData(RenderMesh*);
