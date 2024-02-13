#pragma once 

#include <vector>
#include <string>
#include "engine_config.hpp"

class Texture {
 private:
    unsigned int m_TextureId[MAX_COUNT_TEXTURE];
    int m_Count = 0;

 public:
    
    Texture();

    explicit Texture(std::string diffuse, std::string specular);
    explicit Texture(std::string diffuse);

    void loadImages(std::vector<std::string> paths);

    void loadImage(std::string path);

    int countComponents();

    unsigned int textureId(int idx);

    void bind();
};
