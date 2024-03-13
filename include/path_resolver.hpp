#pragma once

#include <string>
#include "logger.hpp"
#include "engine_config.hpp"

enum class Resource {
    MODEL,
    FONT,
    FSHADER,
    VSHADER,
    TEXTURE,
    SOUND,
    IMAGE
};

std::string GetResourcePath(Resource resource, std::string filename);
