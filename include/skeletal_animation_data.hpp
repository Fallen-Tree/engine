#pragma once

#include <assimp/scene.h>
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "model.hpp"
#include "bone.hpp"
#include "path_resolver.hpp"

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class SkeletalAnimationData {
 public:
    SkeletalAnimationData() = default;

    SkeletalAnimationData(const std::string& animationPath, Model* model) {
        Assimp::Importer importer;
        std::string finalPath = GetResourcePath(Resource::MODEL, animationPath);
        const aiScene* scene = importer.ReadFile(finalPath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Logger::Error("ERROR::ASSIMP::%s", importer.GetErrorString());
            return;
        }
        auto animation = scene->mAnimations[0];
        m_Duration = static_cast<float>(animation->mDuration);
        m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);

        ReadHeirarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    ~SkeletalAnimationData() {
    }

    Bone* FindBone(const std::string& name) {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone) {
                return Bone.GetBoneName() == name;
            });

        for (int i = 0; i < m_Bones.size(); i++) {
            if (m_Bones[i].GetBoneName() == name) {
                return &m_Bones[i];
            }
        }

        return nullptr;
    }


    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration;}

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

 private:
    void ReadMissingBones(const aiAnimation* animation, Model& model) {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model.GetBoneInfoMap();  // getting m_BoneInfoMap from Model class
        int& boneCount = model.GetBoneCount();  // getting the m_BoneCounter from Model class

        // reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            m_Bones.push_back(Bone(channel->mNodeName.data,
                boneInfoMap[channel->mNodeName.data].id, channel));
        }

        m_BoneInfoMap = boneInfoMap;
    }

    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (unsigned int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData newData;
            ReadHeirarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

    float m_Duration;
    float m_TicksPerSecond;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
};
