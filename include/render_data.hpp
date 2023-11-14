#pragma once

#include "model.hpp"
#include "material.hpp"

struct RenderData {
    // TODO(theblek): Replace w/ proper resource-management system w/ handles
    Model *model;
    Material material;
    unsigned int VAO, VBO, EBO;
};
