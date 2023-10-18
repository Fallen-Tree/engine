#include "modelInstance.hpp"

ModelInstance::ModelInstance(Model* model, Transform* transform) {
    this->m_Model = model;
    this->m_Transform = transform;
}
ModelInstance::ModelInstance(Model* model, glm::vec3 translate, glm::vec3 scale, glm::mat4 rotate) {
    this->m_Model = model;
    this->m_Transform = new Transform(translate, scale, rotate);
} 


void ModelInstance::SetModel(Model* model) {
    this->m_Model = model;
}
Model* ModelInstance::GetModel() {
    return this->m_Model;
}


void ModelInstance::SetTransform(Transform* transform) {
    this->m_Transform = transform;
}
Transform* ModelInstance::GetTransform() {
    return this->m_Transform;
}

