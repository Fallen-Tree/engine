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

    SkeletalAnimationData(const std::string& animationPath, unsigned int animationIndex, Model* model) {
        Assimp::Importer importer;
        std::string finalPath = GetResourcePath(Resource::MODEL, animationPath);
        const aiScene* scene = importer.ReadFile(finalPath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Logger::Error("ERROR::ASSIMP::%s", importer.GetErrorString());
            return;
        }

        if (animationIndex >= scene->mNumAnimations) {
            Logger::Error("Too big animation index, can't load animation: %s ", animationPath.c_str());
            return;
        }
        auto animation = scene->mAnimations[animationIndex];
        m_Name +=  animationPath + ":" + scene->mAnimations[animationIndex]->mName.C_Str();
        m_Duration = static_cast<float>(animation->mDuration);
        m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);

        ReadHeirarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    SkeletalAnimationData(const std::string& animationPath, const aiScene* scene,
                                                unsigned int animationIndex, Model* model) {
        if (animationIndex >= scene->mNumAnimations) {
            Logger::Error("Too big animation index, can't load animation");
            return;
        }
        auto animation = scene->mAnimations[animationIndex];
        m_Name = animationPath + ":" + scene->mAnimations[animationIndex]->mName.C_Str();
        m_Duration = static_cast<float>(animation->mDuration);
        m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);

        ReadHeirarchyData(m_RootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    ~SkeletalAnimationData() {
        for (auto it = m_Bones.begin(); it != m_Bones.end(); it++) {
            delete it->second;
        }
    }

    Bone *FindBone(const std::string& name) {
        if (m_Bones.find(name) == m_Bones.end()) {
            return nullptr;
        }

        return m_Bones[name];
    }


    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration;}

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const std::string& GetName() { return m_Name; }

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
            m_Bones[boneName] = (new Bone(channel->mNodeName.data,
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
    std::string m_Name;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    std::map<std::string, Bone*> m_Bones;
    AssimpNodeData m_RootNode;
};
