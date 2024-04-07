#pragma once

#include <assimp/scene.h>
#include <vector>
#include <string>
#include <map>
#include <utility>
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

        ConstructorHelper(animationPath, scene, animationIndex, model);
    }

    SkeletalAnimationData(const std::string& animationPath, const aiScene* scene,
                                                unsigned int animationIndex, Model* model) {
        ConstructorHelper(animationPath, scene, animationIndex, model);
    }

    Bone *FindBone(const std::string& name) {
        if (m_LastBoneIndex >= m_Bones.size()) {
            m_LastBoneIndex = 0;
        }
        if (m_Bones[m_LastBoneIndex].GetBoneID() == -1) {
            m_LastBoneIndex++;
            return nullptr;
        }

        assert(m_Bones[m_LastBoneIndex].GetBoneName() == name);
        return &m_Bones[m_LastBoneIndex++];
    }

    BoneInfo *FindBoneInfo(const std::string& name) {
        if (m_LastBoneInfoIndex >= m_BonesInfos.size()) {
            m_LastBoneInfoIndex = 0;
        }
        assert(m_BonesInfos[m_LastBoneInfoIndex].first == name);
        return m_BonesInfos[m_LastBoneInfoIndex++].second;
    }

    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration;}

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const std::string& GetName() { return m_Name; }

 private:
    void ConstructorHelper(const std::string& animationPath, const aiScene* scene,
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

        std::vector<Bone> newBones;
        ReorderBones(m_RootNode, newBones);
        m_Bones = newBones;
    }

    void ReorderBones(const AssimpNodeData& node, std::vector<Bone> &newBones) {
        bool flag = 0;
        for (int i = 0; i < m_Bones.size(); i++) {
            if (m_Bones[i].GetBoneName() == node.name) {
                newBones.push_back(m_Bones[i]);
                flag = 1;
                break;
            }
        }
        if (!flag) {
            newBones.push_back(Bone());
        }

        if (m_BoneInfoMap.find(node.name) == m_BoneInfoMap.end()) {
            m_BonesInfos.push_back(make_pair(node.name, nullptr));
        } else {
            m_BonesInfos.push_back(make_pair(node.name, &m_BoneInfoMap[node.name]));
        }

        for (int j = 0; j < node.children.size(); j++)
            ReorderBones(node.children[j], newBones);
    }

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
    std::string m_Name;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    std::vector<std::pair<std::string, BoneInfo*> > m_BonesInfos;
    std::vector<Bone> m_Bones;
    int m_LastBoneIndex = 0;
    int m_LastBoneInfoIndex = 0;
    AssimpNodeData m_RootNode;
};
