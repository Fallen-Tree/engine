#include "skeletal_animations_manager.hpp"

SkeletalAnimationsManager::SkeletalAnimationsManager(
        SkeletalAnimationData* animation,
        TPoseType tposetype) {
    m_FinalBoneMatrices.reserve(MAX_BONES);
    for (int i = 0; i < MAX_BONES; i++) {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        m_BonesFrom[i] = (glm::mat4(1.0f));
        m_BonesTo[i] = glm::mat4(1.0f);
        m_BonesTPose[i] = glm::mat4(1.0f);
    }
    m_Animations.push_back(animation);
    m_CurrentAnimationIndex = -1;

    if (tposetype == TPoseType::FROMANIM) {
        CalculateBoneTransformMatrices(
            &(animation->GetRootNode()), glm::mat4(1.0f), m_BonesTPose, animation, 0.0f);
        for (int i = 0; i < MAX_BONES; i++) {
            m_FinalBoneMatrices[i] = m_BonesFrom[i];
        }
    }
}

SkeletalAnimationsManager::SkeletalAnimationsManager(
        const std::string& animationPath,
        Model* model,
        TPoseType tposetype,
        int tposeAnimIndex) {
    m_FinalBoneMatrices.reserve(MAX_BONES);

    m_CurrentAnimationIndex = -1;
    for (int i = 0; i < MAX_BONES; i++) {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        m_BonesFrom[i] = (glm::mat4(1.0f));
        m_BonesTo[i] = glm::mat4(1.0f);
        m_BonesTPose[i] = glm::mat4(1.0f);
    }

    Assimp::Importer importer;
    std::string finalPath = GetResourcePath(Resource::MODEL, animationPath);
    const aiScene* scene = importer.ReadFile(finalPath, aiProcess_Triangulate | aiProcess_OptimizeGraph);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger::Error("ERROR::ASSIMP::%s", importer.GetErrorString());
        return;
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        m_Animations.push_back(new SkeletalAnimationData(animationPath, scene, i, model));
    }

    if (tposetype == TPoseType::FROMANIM) {
        if (tposeAnimIndex >= m_Animations.size()) {
            Logger::Error("Can't use index for TPose Calculating, zero index will be used");
            tposeAnimIndex = 0;
        }
        if (m_Animations.size() == 0) {
            Logger::Error("Can't use zero index for TPose Calculating, no animations found");
            return;
        }
        CalculateBoneTransformMatrices(
            &(m_Animations[tposeAnimIndex]->GetRootNode()),
            glm::mat4(1.0f),
            m_BonesTPose,
            m_Animations[tposeAnimIndex], 0.0f);
        for (int i = 0; i < MAX_BONES; i++) {
            m_FinalBoneMatrices[i] = m_BonesTPose[i];
        }
    }
}

void SkeletalAnimationsManager::AddAnimation(SkeletalAnimationData* animation) {
    m_Animations.push_back(animation);
}

void SkeletalAnimationsManager::AddAnimation(const std::string& animationPath, Model* model) {
    Assimp::Importer importer;
    std::string finalPath = GetResourcePath(Resource::MODEL, animationPath);
    const aiScene* scene = importer.ReadFile(finalPath, aiProcess_Triangulate | aiProcess_OptimizeGraph);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger::Error("ERROR::ASSIMP::%s", importer.GetErrorString());
        return;
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        m_Animations.push_back(new SkeletalAnimationData(animationPath, scene, i, model));
    }
}

std::string SkeletalAnimationsManager::GetAnimationsInfo() {
    std::string info = "\n\n";
    for (int i = 0; i < m_Animations.size(); i++) {
        info += std::to_string(i) + " | " + m_Animations[i]->GetName() + "\n";
    }
    return info;
}


void SkeletalAnimationsManager::PlayImmediately(
        int index, bool looped,
        InterpolationType interpolationType,
        float interpolationTime) {
    if (index < 0 || index >= m_Animations.size()) {
        Logger::Error("Can't play animation, wrong index");
        return;
    }
    m_CurrentAnimationIndex = index;
    m_CurrentTime = 0.f;
    m_Looped = looped;

    if (interpolationType != InterpolationType::NONE) {
        m_Interpolating = 1;
        m_InterpolationType = interpolationType;
        m_InterpolationTime = interpolationTime;
        m_InterpolationTimeLeft = interpolationTime;
        for (int i = 0; i < MAX_BONES; i++) m_BonesFrom[i] = m_FinalBoneMatrices[i];
        CalculateBoneTransformMatrices(
            &(m_Animations[m_CurrentAnimationIndex]->GetRootNode()),
            glm::mat4(1.0f),
            m_BonesTo,
            m_Animations[m_CurrentAnimationIndex],
            m_CurrentTime);
    }
}

bool SkeletalAnimationsManager::IsPlaying() {
    return m_CurrentAnimationIndex != -1 || m_Interpolating;
}

int SkeletalAnimationsManager::GetPlayingAnimationIndex() {
    return m_CurrentAnimationIndex;
}

void SkeletalAnimationsManager::StopImmediately(
    InterpolationType interpolationType, float interpolationTime) {
    if (m_CurrentAnimationIndex == -1) {
        return;
    }

    m_CurrentAnimationIndex = -1;
    m_CurrentTime = 0.f;

    if (interpolationType != InterpolationType::NONE) {
        m_Interpolating = 1;
        m_InterpolationType = interpolationType;
        m_InterpolationTime = interpolationTime;
        m_InterpolationTimeLeft = interpolationTime;
        for (int i = 0; i < MAX_BONES; i++) {
            m_BonesFrom[i] = m_FinalBoneMatrices[i];
            m_BonesTo[i] = m_BonesTPose[i];
        }
    }
}

void SkeletalAnimationsManager::Stop(InterpolationType interpolationType, float interpolationTime) {
    m_Looped = 0;
    m_InterpolationType = interpolationType;
    m_InterpolationTime = interpolationTime;
}

void SkeletalAnimationsManager::Update(float dt) {
    if (m_Interpolating) {
        m_InterpolationTimeLeft -= dt;
        if (m_InterpolationTimeLeft < 0) {
            m_Interpolating = 0;
            for (int i = 0; i < MAX_BONES; i++) {
                m_FinalBoneMatrices[i] = m_BonesTo[i];
            }
            return;
        }
        for (int i = 0; i < MAX_BONES; i++) {
            m_FinalBoneMatrices[i] = m_BonesFrom[i] + (m_BonesTo[i] - m_BonesFrom[i])
                * ((m_InterpolationTime - m_InterpolationTimeLeft) / m_InterpolationTime);
        }
    } else if (m_CurrentAnimationIndex < m_Animations.size() && m_CurrentAnimationIndex >= 0) {
        SkeletalAnimationData* m_CurrentAnimation = m_Animations[m_CurrentAnimationIndex];
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
        if (m_Looped) {
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        } else {
            if (m_CurrentTime >= m_CurrentAnimation->GetDuration()) {
                StopImmediately(m_InterpolationType, m_InterpolationTime);
                return;
            }
        }
        CalculateBoneTransform(&(m_Animations[m_CurrentAnimationIndex]->GetRootNode()), glm::mat4(1.0f));
    }
}

const std::vector<glm::mat4> &SkeletalAnimationsManager::GetFinalBoneMatrices() {
    return m_FinalBoneMatrices;
}

void SkeletalAnimationsManager::CalculateBoneTransform(
        const AssimpNodeData* node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    auto m_CurrentAnimation = m_Animations[m_CurrentAnimationIndex];
    Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;
    auto boneInfo = m_CurrentAnimation->FindBoneInfo(nodeName);
    if (boneInfo != nullptr) {
        int index = boneInfo->id;
        glm::mat4 offset = boneInfo->offset;
        assert(index < MAX_BONES);
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->children.size(); i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}


void SkeletalAnimationsManager::CalculateBoneTransformMatrices(
    const AssimpNodeData* node, glm::mat4 parentTransform,
    glm::mat4 *boneMatrices, SkeletalAnimationData* m_CurrentAnimation, float currentTime) {

    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;
    auto boneInfo = m_CurrentAnimation->FindBoneInfo(nodeName);
    if (boneInfo != nullptr) {
        int index = boneInfo->id;
        glm::mat4 offset = boneInfo->offset;
        assert(index < MAX_BONES);
        boneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->children.size(); i++)
        CalculateBoneTransformMatrices(&node->children[i], globalTransformation,
                                    boneMatrices, m_CurrentAnimation, currentTime);
}
