#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture.hpp"

struct Material{
    float shininess;
    Texture texture;
};
