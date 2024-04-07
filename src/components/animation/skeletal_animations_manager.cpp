#include "skeletal_animations_manager.hpp"

SkeletalAnimationsManager::SkeletalAnimationsManager(SkeletalAnimationData* animation) {
    m_FinalBoneMatrices.reserve(MAX_BONES);
    for (int i = 0; i < MAX_BONES; i++) {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
    m_Animations.push_back(animation);
    m_CurrentAnimationIndex = -1;
}

SkeletalAnimationsManager::SkeletalAnimationsManager(const std::string& animationPath, Model* model) {
    m_FinalBoneMatrices.reserve(MAX_BONES);
    for (int i = 0; i < MAX_BONES; i++) {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
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
    m_CurrentAnimationIndex = -1;
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

void SkeletalAnimationsManager::PlayImmediately(int index, bool looped) {
    if (index < 0 || index >= m_Animations.size()) {
        Logger::Error("Can't play animation, wrong index");
        return;
    }
    m_CurrentAnimationIndex = index;
    m_CurrentTime = 0.0f;
    m_Looped = looped;
}

bool SkeletalAnimationsManager::IsPlaying() {
    return m_CurrentAnimationIndex != -1;
}

void SkeletalAnimationsManager::StopImmediately() {
    m_CurrentTime = 0.0f;
    m_Looped = 0;
    m_CurrentAnimationIndex = -1;
}

void SkeletalAnimationsManager::Stop() {
    m_Looped = 0;
}



void SkeletalAnimationsManager::Update(float dt) {
    if (m_CurrentAnimationIndex < m_Animations.size() && m_CurrentAnimationIndex >= 0) {
        SkeletalAnimationData* m_CurrentAnimation = m_Animations[m_CurrentAnimationIndex];
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
        if (m_Looped) {
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        } else {
            m_CurrentTime = 0.0f;
            for (int i = 0; i < MAX_BONES; i++) {
                m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
            }
            m_CurrentAnimationIndex = -1;
            return;
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
    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        auto boneInfo = boneInfoMap[nodeName];
        int index = boneInfo.id;
        glm::mat4 offset = boneInfo.offset;
        assert(index < MAX_BONES);
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->children.size(); i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}
