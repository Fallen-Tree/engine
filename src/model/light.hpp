#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct EnvLight{
    glm::vec3 m_Position;
    glm::vec3 m_Ambient;
    glm::vec3 m_Diffuse;
    glm::vec3 m_Specular;
};
