#include "transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform(Vec3 translation, Vec3 scale, float radiansDegree,
                     Vec3 rotationAxis) {
    this->m_Translation = translation;
    this->m_Scale = scale;
    this->m_Rotation = glm::rotate(Mat4(1.0), radiansDegree, rotationAxis);
}

Transform::Transform(Vec3 translation, Vec3 scale, Mat4 rotation) {
    this->m_Translation = translation;
    this->m_Scale = scale;
    this->m_Rotation = rotation;
}

// Translation
void Transform::SetTranslation(Vec3 translation) {
    this->m_Translation = translation;
}
void Transform::Translate(Vec3 translate) {
    this->m_Translation += translate;
}

// Scale
void Transform::SetScale(Vec3 scale) {
    this->m_Scale = scale;
}

void Transform::Scale(Vec3 scale) {
    this->m_Scale = this->m_Scale * scale;
}

// Rotate
void Transform::SetRotation(float radiansDegree, Vec3 rotationAxis) {
    this->m_Rotation = glm::rotate(Mat4(1.0), radiansDegree, rotationAxis);
}

void Transform::SetRotation(Mat4 rotationMatrix) {
    this->m_Rotation = rotationMatrix;
}

void Transform::SetRotation(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ) {
    this->m_Rotation = Mat4(1.f);
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegreeX, Vec3(1.0f, 0.f, 0.f));
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegreeY, Vec3(0.0f, 1.f, 0.f));
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegreeZ, Vec3(0.0f, 0.f, 1.f));
}

void Transform::Rotate(float radiansDegree, Vec3 rotationAxis) {
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegree, rotationAxis);
}

void Transform::RotateGlobal(float radiansDegree, Vec3 rotationAxis) {
    auto axis = Mul(rotationAxis, m_Rotation);
    m_Rotation = glm::rotate(m_Rotation, radiansDegree, axis);
}

void Transform::Rotate(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ) {
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegreeX, Vec3(1.0f, 0.f, 0.f));
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegreeY, Vec3(0.0f, 1.f, 0.f));
    this->m_Rotation = glm::rotate(this->m_Rotation, radiansDegreeZ, Vec3(0.0f, 0.f, 1.f));
}

void Transform::RotateGlobal(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ) {
    m_Rotation = glm::rotate(m_Rotation, radiansDegreeX, Mul(Vec3(1.0f, 0.f, 0.f), m_Rotation));
    m_Rotation = glm::rotate(m_Rotation, radiansDegreeY, Mul(Vec3(0.0f, 1.f, 0.f), m_Rotation));
    m_Rotation = glm::rotate(m_Rotation, radiansDegreeZ, Mul(Vec3(0.0f, 0.f, 1.f), m_Rotation));
}

void Transform::Rotate(Mat4 rotationMatrix) {
    this->m_Rotation *= rotationMatrix;
}

// Getters
Vec3 Transform::GetTranslation() {
    return this->m_Translation;
}

Vec3 Transform::GetScale() {
    return this->m_Scale;
}

Mat4 Transform::GetRotation() {
    return this->m_Rotation;
}

Mat4 Transform::GetTransformMatrix() {
    Mat4 transformMatrix(1.0f);
    transformMatrix = glm::translate(transformMatrix, this->m_Translation);
    transformMatrix = transformMatrix * this->m_Rotation;
    transformMatrix = glm::scale(transformMatrix, this->m_Scale);
    return transformMatrix;
}
