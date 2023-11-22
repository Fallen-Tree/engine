#include "logger.hpp"
#include "non_skeletal_animation.hpp"

NonSkeletalAnimation* NonSkeletalAnimation::addAnimation(Transform transform, float time) {
    this->m_AnimationsQueue.push({transform, time});
    return this;
}

NonSkeletalAnimation* NonSkeletalAnimation::stopAnimations() {
    while (!this->m_AnimationsQueue.empty()) {
        m_AnimationsQueue.pop();
    }

    return this;
}

void NonSkeletalAnimation::applyAnimations(Transform* transform, float deltaTime) {
    while (!this->m_AnimationsQueue.empty()) {
        auto current = this->m_AnimationsQueue.front();

        if (deltaTime >= current.second) {
            // Set transform to final Destination and continuing while loop
            transform->SetTransform(current.first);
            deltaTime -= current.second;

            this->m_AnimationsQueue.pop();
        } else {
            // Moving current animation for deltaTime seconds and ending while loop
            // Translate
            transform->SetTranslation(transform->GetTranslation() +
                                      (current.first.GetTranslation() - transform->GetTranslation()) *
                                          (deltaTime / current.second));

            // Rotate
            transform->SetRotation(transform->GetRotation() +
                                     (current.first.GetRotation() - transform->GetRotation()) *
                                         (deltaTime / current.second));

            // Scale
            transform->SetScale(transform->GetScale() +
                                     (current.first.GetScale() - transform->GetScale()) *
                                         (deltaTime / current.second));

            this->m_AnimationsQueue.front().second -= deltaTime;
            break;
        }
    }
}
