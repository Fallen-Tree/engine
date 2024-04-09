#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture.hpp"
#include "math_types.hpp"

struct Material{
    float shininess;
    Texture texture;
    Vec3 diffuseColor;
    Vec3 specularColor;
};
