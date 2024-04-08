#pragma once

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>

#include "render_data.hpp"
#include "shaders.hpp"
#include "transform.hpp"
#include "mesh.hpp"
#include "logger.hpp"
#include "assimp_helpers.hpp"

struct BoneInfo {
    int id;
    glm::mat4 offset;
};

class Model {
 public:
    void processNode(aiNode *node, const aiScene *scene);
    RenderMesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<RenderMesh> meshes;
    ShaderProgram* shader = nullptr;

    void setMaterial(Material material);

    static Model *loadFromFile(std::string);
    static Model *fromMesh(Mesh *mesh, Material material);

    static Model *loadFromFile(std::string, ShaderProgram*);
    static Model *fromMesh(Mesh *mesh, Material material, ShaderProgram*);


    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

 private:
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void SetVertexBoneDataToDefault(Vertex *vertex);
    void SetVertexBoneData(Vertex *vertex, int boneID, float weight);

    void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh* mesh, const aiScene* scene);
};
