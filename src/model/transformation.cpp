#include "transformation.hpp"

Transformation::Transformation(glm::vec3 scale, glm::vec3 rotation, float angle, glm::vec3 translation) {
    this->scale = scale;
    this->rotation = rotation;
    this->translation = translation;
    this->angle = angle;
    this->transform = glm::mat4(1.f);
    this->transform = glm::rotate(this->transform, glm::radians(angle), rotation);
    this->transform = glm::scale(this->transform, this->scale);
    this->transform = glm::translate(this->transform, translation);
}


