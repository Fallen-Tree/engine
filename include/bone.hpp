#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/gtx/quaternion.hpp>
#include "logger.hpp"
#include "assimp_helpers.hpp"

struct KeyPosition {
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

class Bone {
 private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions;
    int m_LastPositionIndex;
    int m_NumRotations;
    int m_LastRotationIndex;
    int m_NumScalings;
    int m_LastScalingIndex;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID = -1;
    float m_LastAnimationTime;

 public:
    Bone() = default;

    Bone(const std::string& name, int ID, const aiNodeAnim* channel) {
        m_NumPositions = channel->mNumPositionKeys;
        m_Name = name;
        m_ID = ID;
        m_LocalTransform = glm::mat4(1.0f);

        m_LastPositionIndex = 0;
        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = static_cast<float>(channel->mPositionKeys[positionIndex].mTime);
            KeyPosition data;
            data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_LastRotationIndex = 0;
        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = static_cast<float>(channel->mRotationKeys[rotationIndex].mTime);
            KeyRotation data;
            data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_LastScalingIndex = 0;
        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex) {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = static_cast<float>(channel->mScalingKeys[keyIndex].mTime);
            KeyScale data;
            data.scale = AssimpGLMHelpers::GetGLMVec(scale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }

    /*interpolates  b/w positions,rotations & scaling keys based on the curren time of 
    the animation and prepares the local transformation matrix by combining all keys 
    tranformations*/
    void Update(float animationTime) {
        if (animationTime < m_LastAnimationTime) {
            m_LastPositionIndex = 0;
            m_LastRotationIndex = 0;
            m_LastScalingIndex = 0;
        }

        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        m_LastAnimationTime = animationTime;
        m_LocalTransform = translation * rotation * scale;
    }

    glm::mat4 GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() { return m_ID; }


    /* Gets the current index on mKeyPositions to interpolate to based on 
    the current animation time*/
    int GetPositionIndex(float animationTime) {
        for (; m_LastPositionIndex < m_NumPositions - 1; ++m_LastPositionIndex) {
            if (animationTime < m_Positions[m_LastPositionIndex + 1].timeStamp)
                return m_LastPositionIndex;
        }
        Logger::Error("SKELETAL ANIM: Can't find current timeStamp");
        assert(0);
        return -1;
    }

    /* Gets the current index on mKeyRotations to interpolate to based on the 
    current animation time*/
    int GetRotationIndex(float animationTime) {
        for (; m_LastRotationIndex < m_NumRotations - 1; ++m_LastRotationIndex) {
            if (animationTime < m_Rotations[m_LastRotationIndex + 1].timeStamp)
                return m_LastRotationIndex;
        }
        Logger::Error("SKELETAL ANIM: Can't find current timeStamp");
        assert(0);
        return -1;
    }


    /* Gets the current index on mKeyScalings to interpolate to based on the 
    current animation time */
    int GetScaleIndex(float animationTime) {
         for (; m_LastScalingIndex < m_NumScalings - 1; ++m_LastScalingIndex) {
            if (animationTime < m_Scales[m_LastScalingIndex + 1].timeStamp)
                return m_LastScalingIndex;
        }
        Logger::Error("SKELETAL ANIM: Can't find current timeStamp");
        assert(0);
        return -1;
    }

 private:
    /* Gets normalized value for Lerp & Slerp*/
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    /*figures out which position keys to interpolate b/w and performs the interpolation 
    and returns the translation matrix*/
    glm::mat4 InterpolatePosition(float animationTime) {
        if (1 == m_NumPositions)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
            m_Positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position,
            m_Positions[p1Index].position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    /*figures out which rotations keys to interpolate b/w and performs the interpolation 
    and returns the rotation matrix*/
    glm::mat4 InterpolateRotation(float animationTime) {
        if (1 == m_NumRotations) {
            auto rotation = glm::normalize(m_Rotations[0].orientation);
            return glm::toMat4(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
            m_Rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation,
            m_Rotations[p1Index].orientation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    /*figures out which scaling keys to interpolate b/w and performs the interpolation 
    and returns the scale matrix*/
    glm::mat4 InterpolateScaling(float animationTime) {
        if (1 == m_NumScalings)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
            m_Scales[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
            , scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
};