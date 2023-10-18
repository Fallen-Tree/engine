#include "transform.hpp"

Transform::Transform(glm::vec3 translation, glm::vec3 scale, float radiansDegree,
                     glm::vec3 rotationAxis) {
    this->m_Translation = translation;
    this->m_Scale = scale;
    this->m_Rotation = glm::rotate(glm::mat4(1.0), radiansDegree, rotationAxis);
}

Transform::Transform(glm::vec3 translation, glm::vec3 scale, glm::mat4 rotation) {
    this->m_Translation = translation;
    this->m_Scale = scale;
    this->m_Rotation = rotation;
}

// Translation
void Transform::SetTranslation(glm::vec3 translation) { 
    this->m_Translation = translation; 
}
void Transform::Translate(glm::vec3 translate) { 
    this->m_Translation += translate; 
}

// Scale
void Transform::SetScale(glm::vec3 scale) { 
    this->m_Scale = scale; 
}

void Transform::Scale(glm::vec3 scale) { 
    this->m_Scale = this->m_Scale * scale; 
}

// Rotate
void Transform::SetRotation(float radiansDegree, glm::vec3 rotationAxis) {
    this->m_Rotation = glm::rotate(glm::mat4(1.0), radiansDegree, rotationAxis);
}

void Transform::SetRotation(glm::mat4 rotationMatrix) { 
    this->m_Rotation = rotationMatrix; 
}

void Transform::SetRotation(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ) {
    this->m_Rotation = glm::mat4(1.f);
    glm::rotate(this->m_Rotation, radiansDegreeX, glm::vec3(1.0f, 0.f, 0.f));
    glm::rotate(this->m_Rotation, radiansDegreeY, glm::vec3(0.0f, 1.f, 0.f));
    glm::rotate(this->m_Rotation, radiansDegreeZ, glm::vec3(0.0f, 0.f, 1.f));
}

void Transform::Rotate(float radiansDegree, glm::vec3 rotationAxis) {
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegree, rotationAxis);
}

void Transform::Rotate(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ) {
    glm::rotate(this->m_Rotation, radiansDegreeX, glm::vec3(1.0f, 0.f, 0.f));
    glm::rotate(this->m_Rotation, radiansDegreeY, glm::vec3(0.0f, 1.f, 0.f));
    glm::rotate(this->m_Rotation, radiansDegreeZ, glm::vec3(0.0f, 0.f, 1.f));
}

// Getters
glm::vec3 Transform::GetTranslation() { 
    return this->m_Translation; 
}

glm::vec3 Transform::GetScale() { 
    return this->m_Scale; 
}

glm::mat4 Transform::GetRotation() { 
    return this->m_Rotation; 
}

glm::mat4 Transform::GetTransformMatrix() {
    glm::mat4 transformMatrix(1.0f);
    transformMatrix = glm::translate(transformMatrix, this->m_Translation);
    transformMatrix = transformMatrix * this->m_Rotation;
    transformMatrix = glm::scale(transformMatrix, this->m_Scale);
    return transformMatrix;
}
