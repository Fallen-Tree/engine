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

    // PointLight() {}
    PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
            glm::vec3 position, float constDistCoeff, float linearDistCoeff,
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
    glm::vec3 direction;

    DirLight(glm::vec3 ambient, glm::vec3 diffuse,
            glm::vec3 specular, glm::vec3 direction) {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->direction = direction;
    }
};

struct SpotLight : Light {
    glm::vec3 position;
    float constDistCoeff;
    float linearDistCoeff;
    float quadraticDistCoeff;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
            glm::vec3 position, float constDistCoeff, float linearDistCoeff,
            float quadraticDistCoeff, glm::vec3 direction, float cutOff,
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
