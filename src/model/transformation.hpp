#ifndef SRC_MODEL_TRANSFORMATION_HPP_
#define SRC_MODEL_TRANSFORMATION_HPP_

#include <glm/glm.hpp>
#include "model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transformation {
 public:
    float angle;
    glm::vec3 translation;
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::mat4 transform;


    Transformation(glm::vec3 scale, glm::vec3 rotation, float angle, glm::vec3 tranlsation);
};

#endif  // SRC_MODEL_TRANSFORMATION_HPP_
