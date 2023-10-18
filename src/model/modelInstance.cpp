#include "modelInstance.hpp"

ModelInstance::ModelInstance
    (glm::vec3 scale, glm::vec3 rotationAxis, float radiansDegree, glm::vec3 translation, Model* model) {
    this->scale = scale;
    this->rotationAxis = rotationAxis;
    this->translation = translation;
    this->radiansDegree = radiansDegree;
    this->model = model;
    update();
}

void ModelInstance::update() {
    this->transform = glm::mat4(1.f);
    this->transform = glm::translate(this->transform, translation);
    this->transform = glm::rotate(this->transform, radiansDegree, rotationAxis);
    this->transform = glm::scale(this->transform, this->scale);
}

void ModelInstance::setModel(Model* model) {
    this->model = model;
}

void ModelInstance::setScale(glm::vec3 scale) {
    this->scale = scale;
    update();
}

void ModelInstance::setRotation(glm::vec3 rotationAxis, float radiansDegree) {
    this->rotationAxis = rotationAxis;
    this->radiansDegree = radiansDegree;
    update();
}

void ModelInstance::setTranslation(glm::vec3 translation) {
    this->translation = translation;
    update();
}

Model* ModelInstance::getModel() {
    return this->model;
}



