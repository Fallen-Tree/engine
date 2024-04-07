#pragma once

#include <assimp/scene.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>

#include "bone.hpp"
#include "skeletal_animation_data.hpp"

#define MAX_BONES 100

class SkeletalAnimationsManager {
 public:
    SkeletalAnimationsManager() = default;

    explicit SkeletalAnimationsManager(const std::string& animationPath, Model* model);
    explicit SkeletalAnimationsManager(SkeletalAnimationData* animation);

    void AddAnimation(SkeletalAnimationData* animation);
    void AddAnimation(const std::string& animationPath, Model* model);

    std::string GetAnimationsInfo();

    void PlayImmediately(int id, bool looped);
    bool IsPlaying();

    void StopImmediately();
    void Stop();

    // Engine functions
    void Update(float dt);

    const std::vector<glm::mat4> &GetFinalBoneMatrices();

 private:
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::vector<SkeletalAnimationData*> m_Animations;
    int m_CurrentAnimationIndex;
    float m_CurrentTime;
    bool m_Looped;
};


