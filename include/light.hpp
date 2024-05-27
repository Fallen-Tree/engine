#pragma once

#include <variant>
#include "math_types.hpp"
#include "engine_config.hpp"
#include <glad/glad.h>

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

    PointLight() = default;
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
    unsigned int depthFBO;
    unsigned int depthMap;

    DirLight() = default;
    DirLight(Vec3 ambient, Vec3 diffuse,
            Vec3 specular, Vec3 direction) {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->direction = direction;

        glGenFramebuffers(1, &depthFBO);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    SpotLight() = default;
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

typedef std::variant<PointLight *, SpotLight *, DirLight *> LightSource;
