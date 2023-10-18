#include "transform.hpp"

Transform::Transform(glm::vec3 translate, glm::vec3 scale, glm::vec3 rotationAxis, float radiansDegree) {
  this->m_Translate = translate;
  this->m_Scale = scale;
  this->m_Rotate = glm::rotate(glm::mat4(1.0), radiansDegree, rotationAxis);
}

Transform::Transform(glm::vec3 translate, glm::vec3 scale, glm::mat4 rotate) {
  this->m_Translate = translate;
  this->m_Scale = scale;
  this->m_Rotate = rotate;
}

// Translation
void Transform::SetTranslate(glm::vec3 translate) {
  this->m_Translate = translate;
}
void Transform::ModifyTranslate(glm::vec3 shift) {
  this->m_Translate += shift;
}

// Scale
void Transform::SetScale(glm::vec3 scale) {
  this->m_Scale = scale;
}
void Transform::ModifyScale(glm::vec3 scaleShift) {
  this->m_Scale = this->m_Scale * scaleShift; 
}

// Rotate
void Transform::SetRotate(float radiansDegree, glm::vec3 rotationAxis) {
  this->m_Rotate = glm::rotate(glm::mat4(1.0), radiansDegree, rotationAxis);
}

void Transform::SetRotate(glm::mat4 rotate) {
  this->m_Rotate = rotate;
}

void Transform::ModifyRotate(float radiansDegree, glm::vec3 rotationAxis) {
  this->m_Rotate = glm::rotate(this->m_Rotate, radiansDegree, rotationAxis);
}


// Getters
glm::vec3 Transform::GetTranslate() {
  return this->m_Translate;
};
glm::vec3 Transform::GetScale() {
  return this->m_Scale;
}
glm::mat4 Transform::GetRotate() {
  return this->m_Rotate;
}
glm::mat4 Transform::GetTransformMatrix() {
  glm::mat4 transform(1.0);
  transform = glm::translate(transform, this->m_Translate);
  transform = transform * this->m_Rotate; 
  transform = glm::scale(transform, this->m_Scale);
  return transform;
}