#pragma once

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <assimp/Importer.hpp>

#include "render_data.hpp"
#include "shaders.hpp"

class Model {
 private:
    void processNode(aiNode *node, const aiScene *scene);
    RenderMesh processMesh(aiMesh *mesh, const aiScene *scene);

 public:
    std::vector<RenderMesh> meshes;
    ShaderProgram* shader = nullptr;

    // constructor of class.
    static Model* loadFromFile(const char* path);
};
