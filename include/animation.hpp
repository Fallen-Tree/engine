  #pragma once

#include <glm/glm.hpp>
#include <queue>

#include "transform.hpp"


class Animation {
 private:
  std::queue<std::pair<Transform, float>> m_AnimationsQueue; // pair<destinationTransform, time of anim>

 public:
    Animation* addAnimation(Transform transform, float time);
    Animation* stopAnimations();

    void applyAnimations(Transform*, float);
};

