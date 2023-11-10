#include <glad/glad.h>
#include <iostream>
#include "texture.hpp"
#include "stb_image.h"
#include "config.hpp"

Texture::Texture() {}

Texture::Texture(std::vector<std::string> paths) {
    loadImage(paths);
}

Texture::Texture(std::string path) {
    loadImage(path);
}

void Texture::loadImage(std::vector<std::string> paths) {
    for (int i = 0; i < paths.size(); i++) {
        loadImage(paths[i]);
    }
}

void Texture::loadImage(std::string path) {
    if (m_Count >= MaxCountTexture) {
        std::cout << "failed to load texture at path" << path
            << "because of overflow buffer" << std::endl;
        return;
    }

    int width, height, nrComponents;
    char finalPath[512];
    snprintf(finalPath, sizeof(finalPath),
        reinterpret_cast<const char*>
        ((RESOURCE_DIR"/textures%s")), path.c_str());
    unsigned char *data = reinterpret_cast<unsigned char*>(
        stbi_load(finalPath,
        &width, &height, &nrComponents, 0));

    if (!data) {
        stbi_image_free(data);
        std::cout << "Texture failed to load at path: " << finalPath << std::endl;
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
