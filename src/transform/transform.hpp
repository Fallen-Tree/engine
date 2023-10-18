#ifndef SRC_TRANSFORM_TRANSFORM_HPP_
#define SRC_TRANSFORM_TRANSFORM_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform {
 private:
    glm::vec3 m_Translate;
    glm::vec3 m_Scale;
    glm::mat4 m_Rotate;

    glm::mat4 m_Transform;

 public:
    explicit Transform(glm::vec3 translation, glm::vec3 scale, glm::vec3 rotationAxis, float radiansDegree);
    explicit Transform(glm::vec3 translation, glm::vec3 scale, glm::mat4 rotate);

    // Translation
    void SetTranslate(glm::vec3 translate);
    void ModifyTranslate(glm::vec3 shift);

    // Scale
    void SetScale(glm::vec3 scale);
    void ModifyScale(glm::vec3 scaleShift);

    // Rotate
    void SetRotate(float radiansDegree, glm::vec3 rotationAxis);
    void SetRotate(glm::mat4 rotate);
    void ModifyRotate(float radiansDegree, glm::vec3 rotationAxis);


    // Getters
    glm::vec3 GetTranslate();
    glm::vec3 GetScale();
    glm::mat4 GetRotate();
    glm::mat4 GetTransformMatrix();
};

#endif  // SRC_TRANSFORM_TRANSFORM_HPP_
