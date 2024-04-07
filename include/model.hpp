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

    void SetVertexBoneDataToDefault(Vertex *vertex) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            vertex->m_BoneIDs[i] = -1;
            vertex->m_Weights[i] = 0.0f;
        }
    }

    void SetVertexBoneData(Vertex *vertex, int boneID, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (vertex->m_BoneIDs[i] < 0) {
                vertex->m_Weights[i] = weight;
                vertex->m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh* mesh, const aiScene* scene) {
        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
                BoneInfo newBoneInfo;
                newBoneInfo.id = m_BoneCounter;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                    mesh->mBones[boneIndex]->mOffsetMatrix);
                m_BoneInfoMap[boneName] = newBoneInfo;
                boneID = m_BoneCounter;
                m_BoneCounter++;
            } else {
                boneID = m_BoneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(&vertices[vertexId], boneID, weight);
            }
        }
    }
};
