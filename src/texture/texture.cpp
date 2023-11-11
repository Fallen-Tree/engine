#include <glad/glad.h>
#include <iostream>
#include "texture.hpp"
#include "stb_image.h"
#include "config.hpp"
#include "logger.hpp"

Texture::Texture() {}

Texture::Texture(std::vector<std::string> paths) {
    loadImages(paths);
}

Texture::Texture(std::string path) {
    loadImage(path);
}

void Texture::loadImages(std::vector<std::string> paths) {
    for (int i = 0; i < paths.size(); i++) {
        loadImage(paths[i]);
    }
}

void Texture::loadImage(std::string path) {
    if (m_Count >= MaxCountTexture) {
        Logger::Error(
            "SHADER::PROGRAM::FAILED_TO_LOAD_TEXTURE_AT_PATH_%s_BECAUSE_OVERFLOW\n", path);
        return;
    }

    int width, height, nrComponents;
    char finalPath[512];
    snprintf(finalPath, sizeof(finalPath),
        reinterpret_cast<const char*>
        ((RESOURCE_DIR"/textures%s")), path.c_str());
    unsigned char *data = reinterpret_cast<unsigned char*>(
        stbi_load(finalPath, &width, &height, &nrComponents, 0));

    if (!data) {
        stbi_image_free(data);
        Logger::Error("SHADER::PROGRAM::FILE_NOT_FOUND_FAILED: %s\n", finalPath);
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
        std::cout << "index out of bounds" << std::endl;
        Logger::Error("SHADER::PROGRAM:INDEX_OUT_OF_BOUNDS\n");
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

