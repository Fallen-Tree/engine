#pragma once

#include "model.hpp"
#include "material.hpp"

struct RenderData {
    // TODO(theblek): Replace w/ proper resource-management system w/ handles
    Model *m_Model;
    Material m_Mat;
    unsigned int VAO, VBO, EBO;
};
