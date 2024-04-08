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

    SkeletalAnimationData(const std::string& animationPath, unsigned int animationIndex, Model* model);
    SkeletalAnimationData(const std::string& animationPath, const aiScene* scene,
                                                            unsigned int animationIndex, Model* model);

    Bone *FindBone(const std::string& name);
    BoneInfo *FindBoneInfo(const std::string& name);

    float GetTicksPerSecond();
    float GetDuration();
    const AssimpNodeData& GetRootNode();
    const std::string& GetName();

 private:
    void ConstructorHelper(const std::string& animationPath, const aiScene* scene,
                                                unsigned int animationIndex, Model* model);

    void ReorderBones(const AssimpNodeData& node, std::vector<Bone> &newBones);
    void ReadMissingBones(const aiAnimation* animation, Model& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

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
