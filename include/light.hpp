#pragma once

#include "math_types.hpp"

struct Light {
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
};

struct PointLight : Light {
    Vec3 position;
    float constDistCoeff;
    float linearDistCoeff;
    float quadraticDistCoeff;
};

struct DirLight : Light {
    Vec3 direction;
};

struct SpotLight : PointLight {
    Vec3 direction;
    float cutOff;
    float outerCutOff;
};
