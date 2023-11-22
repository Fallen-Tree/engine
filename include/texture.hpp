#pragma once 

#include <vector>
#include <string>
#include "engine_config.hpp"

class Texture {
 private:
    unsigned int m_TextureId[MAXCOUNTTEXTURE];
    int m_Count = 0;

 public:
    
    Texture();

    explicit Texture(std::vector<std::string> paths);

    explicit Texture(std::string path);

    void loadImages(std::vector<std::string> paths);

    void loadImage(std::string path);  

    int countComponents();

    unsigned int textureId(int idx);

    void bind();
};
