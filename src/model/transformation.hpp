#ifndef SRC_MODEL_TRANSFORMATION_HPP_
#define SRC_MODEL_TRANSFORMATION_HPP_

#include <glm/glm.hpp>
#include "model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transformation {
 private:
    float angle;
    glm::vec3 translate;
    glm::vec3 scale;
    glm::vec3 rotate;
    Model *model;

    void makeTransform();

 public:
    glm::mat4 transform;

    Transformation(glm::vec3 scale, glm::vec3 rotate, float angle, glm::vec3 tranlsate, Model * model);

    void setTranslate(glm::vec3 translate);

    void setScale(glm::vec3 scale);

    void setRotate(glm::vec3 rotate, float angle);

    void setModel(Model* model);

    Model* getModel();
};

#endif  // SRC_MODEL_TRANSFORMATION_HPP_
