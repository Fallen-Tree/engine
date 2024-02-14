#include <glad/glad.h>
#include <iostream>
#include "texture.hpp"
#include "stb_image.h"
#include "engine_config.hpp"
#include "logger.hpp"

Texture::Texture() {}

Texture::Texture(std::string diffuse, std::string specular) {
    loadImage(diffuse);
    loadImage(specular);
}

Texture::Texture(std::string diffuse) {
    loadImage(diffuse);
    loadImage(diffuse);
}

void Texture::loadImages(std::vector<std::string> paths) {
    for (int i = 0; i < paths.size(); i++) {
        loadImage(paths[i]);
    }
}

void Texture::loadImage(std::string path) {
    if (m_Count >= MAX_COUNT_TEXTURE) {
        Logger::Error(
            "TEXTURE::PROGRAM::LOADER::FAILED_TO_LOAD_TEXTURE_AT_PATH_%s_BECAUSE_OVERFLOW", path.c_str());
        return;
    }

    int width, height, nrComponents;
    unsigned char *data = reinterpret_cast<unsigned char*>(
        stbi_load(path.c_str(), &width, &height, &nrComponents, 0));

    if (!data) {
        stbi_image_free(data);
        Logger::Error("TEXTURE::LOADER::PROGRAM::FILE_NOT_FOUND_FAILED: %s", path.c_str());
        return;
    }
    GLenum format = GL_RGBA;

    glGenTextures(1, &m_TextureId[m_Count]);

    glBindTexture(GL_TEXTURE_2D, m_TextureId[m_Count]);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    m_Count++;
}

int Texture::countComponents() {
    return m_Count;
}

unsigned int Texture::textureId(int idx) {
    if (idx < 0 || idx > m_Count) {
        Logger::Error("TEXTURE::PROGRAM:INDEX_OUT_OF_BOUNDS");
        exit(1);
    }
    return m_TextureId[idx];
}

void Texture::bind() {
    for (int i = 0; i < m_Count; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureId[i]);
    }
}

