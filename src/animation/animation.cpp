#include "logger.hpp"
#include "animation.hpp"

Animation* Animation::addAnimation(Transform transform, float time) {
    m_AnimationsQueue.push({transform, time});
    return this;
}

Animation* Animation::stopAnimations() {
    while (!m_AnimationsQueue.empty()) {
        m_AnimationsQueue.pop();
    }

    return this;
}

void Animation::applyAnimations(Transform* transform, float deltaTime) {
    // Process completely finished animations
    while (!m_AnimationsQueue.empty()) {
        auto currentAnim = m_AnimationsQueue.front();
        if (deltaTime >= currentAnim.second) {
            *transform = currentAnim.first;
            deltaTime -= currentAnim.second;

            m_AnimationsQueue.pop();
        } else {
            break;
        }
    }

    if (m_AnimationsQueue.empty()) return;

    auto currentAnim = m_AnimationsQueue.front();
    float timeCoeff = deltaTime / currentAnim.second;

    // Translate
    glm::vec3 translateDiff = currentAnim.first.GetTranslation() - transform->GetTranslation();
    transform->SetTranslation(transform->GetTranslation() + translateDiff * timeCoeff);

    // Rotate
    glm::mat4 rotateDiff = currentAnim.first.GetRotation() - transform->GetRotation();
    transform->SetRotation(transform->GetRotation() + rotateDiff * timeCoeff);

    // Scale
    glm::vec3 scaleDiff = currentAnim.first.GetScale() - transform->GetScale();
    transform->SetScale(transform->GetScale() + scaleDiff * timeCoeff);

    m_AnimationsQueue.front().second -= deltaTime;
}
