#include "skeletal_animations_manager.hpp"

SkeletalAnimationsManager::SkeletalAnimationsManager(SkeletalAnimationData* animation) {
    m_CurrentAnimation = animation;
    m_CurrentTime = 0.0f;

    m_FinalBoneMatrices.reserve(MAX_BONES);
    for (int i = 0; i < MAX_BONES; i++) {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void SkeletalAnimationsManager::UpdateAnimation(float dt) {
    if (m_CurrentAnimation) {
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void SkeletalAnimationsManager::PlayAnimation(SkeletalAnimationData* pAnimation) {
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0f;
}

const std::vector<glm::mat4> &SkeletalAnimationsManager::GetFinalBoneMatrices() {
    return m_FinalBoneMatrices;
}

void SkeletalAnimationsManager::CalculateBoneTransform(
        const AssimpNodeData* node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;
    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}
