#pragma once

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>

#include "render_data.hpp"
#include "shaders.hpp"
#include "mesh.hpp"

struct BoneInfo {
    int id;
    glm::mat4 offset;
};

class Model {
 public:
    std::vector<RenderMesh> meshes;
    ShaderProgram shader;
    ShaderProgram depthShader;

    void setMaterial(Material material);

    static Model *fromMesh(Mesh *mesh, Material material);
    static Model *fromMesh(Mesh *mesh, Material material, ShaderProgram);

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

    friend class ModelManager;
 private:
    static Model *loadFromFile(std::string, ShaderProgram);
    static Model *loadFromFile(std::string);

    void processNode(aiNode *node, const aiScene *scene);
    RenderMesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void SetVertexBoneDataToDefault(Vertex *vertex);
    void SetVertexBoneData(Vertex *vertex, int boneID, float weight);

    void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh* mesh, const aiScene* scene);
};
