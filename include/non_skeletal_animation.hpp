#pragma once

#include <glm/glm.hpp>
#include <queue>

#include "transform.hpp"


class NonSkeletalAnimation {
 private:
  std::queue<std::pair<Transform, float>> m_AnimationsQueue; // pair<destinationTransform, time of anim>

 public:
    NonSkeletalAnimation* addAnimation(Transform transform, float time);
    NonSkeletalAnimation* stopAnimations();

    void applyAnimations(Transform*, float);
};

