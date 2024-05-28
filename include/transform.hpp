#ifndef SRC_TRANSFORM_TRANSFORM_HPP_
#define SRC_TRANSFORM_TRANSFORM_HPP_

#include "math_types.hpp"

class Transform {
 private:
    Vec3 m_Translation;
    Vec3 m_Scale;
    Mat4 m_Rotation;

 public:
    Transform() = default;
    explicit Transform(Vec3 translation, Vec3 scale, float radiansDegree, Vec3 rotationAxis);
    explicit Transform(Vec3 translation, Vec3 scale, Mat4 rotationMatrix);

    // Translation
    void SetTranslation(Vec3 translation);
    void Translate(Vec3 shift);

    // Scale
    void SetScale(Vec3 scale);
    void Scale(Vec3 scale);

    // Rotate
    void SetRotation(float radiansDegree, Vec3 rotationAxis);
    void SetRotation(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ);
    void SetRotation(Mat4 rotationMatrix);

    void Rotate(float radiansDegree, Vec3 rotationAxis);
    void Rotate(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ);
    void Rotate(Mat4 rotationMatrix);

    void RotateGlobal(float radiansDegree, Vec3 rotationAxis);
    void RotateGlobal(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ);

    // Getters
    Vec3 GetTranslation() const;
    Vec3 GetScale() const;
    Mat4 GetRotation() const;
    Mat4 GetTransformMatrix() const;
};

#endif  // SRC_TRANSFORM_TRANSFORM_HPP_
