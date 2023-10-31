#ifndef SRC_MODEL_MODELINSTANCE_HPP_
#define SRC_MODEL_MODELINSTANCE_HPP_

#include "model.hpp"
#include "transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "material.hpp"

class ModelInstance {
 private:
    Model* m_Model;
    Transform* m_Transform;
    

 public:
    Material m_Mat;
    explicit ModelInstance(Model* model, Transform* transform);
    explicit ModelInstance(Model* model, glm::vec3 translate, glm::vec3 scale, glm::mat4 rotate);

    void SetModel(Model* model);
    Model* GetModel();

    void SetTransform(Transform* transform);
    Transform* GetTransform();
};

#endif  // SRC_MODEL_MODELINSTANCE_HPP_
