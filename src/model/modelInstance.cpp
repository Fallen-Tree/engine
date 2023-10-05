#include "modelInstance.hpp"

ModelInstance::ModelInstance
    (glm::vec3 scale, glm::vec3 rotationAxis, float angleDegree, glm::vec3 translation, Model* model) {
    this->scale = scale;
    this->rotationAxis = rotationAxis;
    this->translation = translation;
    this->angleDegree = angleDegree;
    this->model = model;
    update();
}

void ModelInstance::update() {
    this->transform = glm::mat4(1.f);
    this->transform = glm::translate(this->transform, translation);
    this->transform = glm::scale(this->transform, this->scale);
    this->transform = glm::rotate(this->transform, glm::radians(angleDegree), rotationAxis);
}

void ModelInstance::setModel(Model* model) {
    this->model = model;
}

void ModelInstance::setScale(glm::vec3 scale) {
    this->scale = scale;
    update();
}

void ModelInstance::setRotation(glm::vec3 rotationAxis, float angleDegree) {
    this->rotationAxis = rotationAxis;
    this->angleDegree = angleDegree;
    update();
}

void ModelInstance::setTranslation(glm::vec3 translation) {
    this->translation = translation;
    update();
}

Model* ModelInstance::getModel() {
    return this->model;
}



