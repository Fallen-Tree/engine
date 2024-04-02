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
    SkeletalAnimationsManager() {
        m_CurrentAnimation = nullptr;
        m_CurrentTime = 0.0f;
    }
    explicit SkeletalAnimationsManager(SkeletalAnimationData* animation);

    void UpdateAnimation(float dt);

    void PlayAnimation(SkeletalAnimationData* pAnimation);

    const std::vector<glm::mat4> &GetFinalBoneMatrices();

 private:
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    std::vector<glm::mat4> m_FinalBoneMatrices;
    SkeletalAnimationData* m_CurrentAnimation;
    float m_CurrentTime;
};


