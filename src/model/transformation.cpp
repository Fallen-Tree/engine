#include "transformation.hpp"

Transformation::Transformation
    (glm::vec3 scale, glm::vec3 rotate, float angle, glm::vec3 translate, Model* model) {
    this->scale = scale;
    this->rotate = rotate;
    this->translate = translate;
    this->angle = angle;
    this->model = model;
    makeTransform();
}

void Transformation::makeTransform() {
    this->transform = glm::mat4(1.f);
    this->transform = glm::rotate(this->transform, glm::radians(angle), rotate);
    this->transform = glm::scale(this->transform, this->scale);
    this->transform = glm::translate(this->transform, translate);
}

void Transformation::setModel(Model* model) {
    this->model = model;
    makeTransform();
}

void Transformation::setScale(glm::vec3 scale) {
    this->scale = scale;
    makeTransform();
}

void Transformation::setRotate(glm::vec3 rotate, float angle) {
    this->rotate = rotate;
    this->angle = angle;
    makeTransform();
}

void Transformation::setTranslate(glm::vec3 translate) {
    this->translate = translate;
    makeTransform();
}

Model* Transformation::getModel() {
    return this->model;
}



