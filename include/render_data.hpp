#pragma once

#include "model.hpp"
#include "material.hpp"

struct RenderData {
    // TODO(theblek): Replace w/ proper resource-management system w/ handles
    Model *model;
    Material material;
    unsigned int VAO, VBO, EBO;

    explicit RenderData(Model* model) {
        this->model = model;
        bindRenderData(this);
    }

    RenderData(Model* model, Material material) {
        this->model = model;
        this->material = material;
        bindRenderData(this);
    }

    void bindRenderData(RenderData*);
};


