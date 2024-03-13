#pragma once

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>

#include "render_data.hpp"
#include "shaders.hpp"
#include "transform.hpp"

class Model {
 private:
    void processNode(aiNode *node, const aiScene *scene);
    RenderMesh processMesh(aiMesh *mesh, const aiScene *scene);

 public:
    std::vector<RenderMesh> meshes;
    ShaderProgram* shader = nullptr;

    static Model *loadFromFile(std::string);
    static Model *fromMesh(Mesh *mesh, Material material);
};
