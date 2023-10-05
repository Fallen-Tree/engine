#ifndef SRC_MODEL_MODELINSTANCE_HPP_
#define SRC_MODEL_MODELINSTANCE_HPP_

#include <glm/glm.hpp>
#include "model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ModelInstance {
 private:
    // in degree!!
    float angleDegree;
    glm::vec3 translation;
    glm::vec3 scale;
    // rotate shows which axis to turn along
    // for example: if rotate = (0.f, 0.f, 1.f), then turn along z
    glm::vec3 rotationAxis;
    Model *model;

    void makeTransform();

 public:
    glm::mat4 transform;

    explicit ModelInstance(glm::vec3 scale,
      glm::vec3 rotation_axis, float angleDegree, glm::vec3 tranlsation, Model * model);

    void setTranslate(glm::vec3 translation);

    void setScale(glm::vec3 scale);

    void setRotation(glm::vec3 rotationAxis, float angleDegree);

    void setModel(Model* model);

    Model* getModel();
};

#endif  // SRC_MODEL_MODELINSTANCE_HPP_
