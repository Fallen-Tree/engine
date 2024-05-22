#pragma once

#include <assimp/scene.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>

#include "bone.hpp"
#include "skeletal_animation_data.hpp"
#include "engine_config.hpp"

/*
   Let's call TPose - initial model position, without any animations
*/

// Interpolation types, none - no interpolation at all
enum class InterpolationType {
    LINEAR,
    NONE
};

enum class TPoseType {
    BASIC,
    FROMANIM
};

class SkeletalAnimationsManager {
 public:
    SkeletalAnimationsManager() = default;

    explicit SkeletalAnimationsManager(
        const std::string& animationPath,
        Model* model,
        TPoseType tposetype = TPoseType::BASIC,
        int tposeAnimIndex = 0);
    explicit SkeletalAnimationsManager(
        SkeletalAnimationData* animation,
        TPoseType tposetype = TPoseType::BASIC);

    void AddAnimation(SkeletalAnimationData* animation);
    void AddAnimation(const std::string& animationPath, Model* model);

    std::string GetAnimationsInfo();

    // TPose or Animation -> Animation Immideately
    void PlayImmediately(
        int id, bool looped,
        InterpolationType interpolationType = InterpolationType::LINEAR,
        float interpolationTime = 0.2f);

    // Animation -> TPose
    void StopImmediately(
        InterpolationType interpolationType = InterpolationType::LINEAR,
        float interpolationTime = 0.2f);

    void Stop(InterpolationType interpolationType = InterpolationType::LINEAR,
        float interpolationTime = 0.2f);

    // Is animating or interpolating
    bool IsPlaying();
    // If interpolating -> -1
    int GetPlayingAnimationIndex();

    // Engine functions
    void Update(float dt);
    const std::vector<glm::mat4> &GetFinalBoneMatrices();

 private:
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
    void CalculateBoneTransformMatrices(
        const AssimpNodeData* node, glm::mat4 parentTransform,
        std::vector<glm::mat4> &boneMatrices,
        SkeletalAnimationData* m_CurrentAnimation, float);

    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::vector<SkeletalAnimationData*> m_Animations;

    int m_CurrentAnimationIndex;
    float m_CurrentTime;
    bool m_Looped;

    // Interpolation data
    bool m_Interpolating = 0;
    float m_InterpolationTime;
    float m_InterpolationTimeLeft;
    InterpolationType m_InterpolationType;
    std::vector<glm::mat4> m_BonesFrom;
    std::vector<glm::mat4> m_BonesTo;
    std::vector<glm::mat4> m_BonesTPose;
};
