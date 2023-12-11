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

    // PointLight() {}
    PointLight(Vec3 ambient, Vec3 diffuse, Vec3 specular,
            Vec3 position, float constDistCoeff, float linearDistCoeff,
            float quadraticDistCoeff) {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->position = position;
        this->constDistCoeff = constDistCoeff;
        this->linearDistCoeff = linearDistCoeff;
        this->quadraticDistCoeff = quadraticDistCoeff;
    }
};

struct DirLight : Light {
    Vec3 direction;

    DirLight(Vec3 ambient, Vec3 diffuse,
            Vec3 specular, Vec3 direction) {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->direction = direction;
    }
};

struct SpotLight : Light {
    Vec3 position;
    float constDistCoeff;
    float linearDistCoeff;
    float quadraticDistCoeff;
    Vec3 direction;
    float cutOff;
    float outerCutOff;

    SpotLight(Vec3 ambient, Vec3 diffuse, Vec3 specular,
            Vec3 position, float constDistCoeff, float linearDistCoeff,
            float quadraticDistCoeff, Vec3 direction, float cutOff,
            float outerCutOff) {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->position = position;
        this->constDistCoeff = constDistCoeff;
        this->linearDistCoeff = linearDistCoeff;
        this->quadraticDistCoeff = quadraticDistCoeff;
        this->direction = direction;
        this->cutOff = cutOff;
        this->outerCutOff = outerCutOff;
    }
};
