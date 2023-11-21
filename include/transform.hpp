#ifndef SRC_TRANSFORM_TRANSFORM_HPP_
#define SRC_TRANSFORM_TRANSFORM_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform {
 private:
    glm::vec3 m_Translation;
    glm::vec3 m_Scale;
    glm::mat4 m_Rotation;

 public:
    Transform() = default;
    explicit Transform(glm::vec3 translation, glm::vec3 scale, float radiansDegree, glm::vec3 rotationAxis);
    explicit Transform(glm::vec3 translation, glm::vec3 scale, glm::mat4 rotationMatrix);
    
    void SetTransform(Transform transform);

    // Translation
    void SetTranslation(glm::vec3 translation);
    void Translate(glm::vec3 shift);

    // Scale
    void SetScale(glm::vec3 scale);
    void Scale(glm::vec3 scale);

    // Rotate
    void SetRotation(float radiansDegree, glm::vec3 rotationAxis);
    void SetRotation(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ);
    void SetRotation(glm::mat4 rotationMatrix);
    void Rotate(float radiansDegree, glm::vec3 rotationAxis);
    void Rotate(float radiansDegreeX, float radiansDegreeY, float radiansDegreeZ);
    void Rotate(glm::mat4 rotationMatrix);


    // Getters
    glm::vec3 GetTranslation();
    glm::vec3 GetScale();
    glm::mat4 GetRotation();
    glm::mat4 GetTransformMatrix();
};

#endif  // SRC_TRANSFORM_TRANSFORM_HPP_
