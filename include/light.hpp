#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Light {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight : Light {
    glm::vec3 position;
    float constDistCoeff;
    float linearDistCoeff;
    float quadraticDistCoeff;
};

struct DirLight : Light {
    glm::vec3 direction;
};

struct SpotLight : PointLight {
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
};
