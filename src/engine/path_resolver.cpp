#include "path_resolver.hpp"

std::string GetResourcePath(Resource resource, std::string filename) {
    std::string prefix;
    switch (resource) {
        case Resource::MODEL:
            prefix = "/models/";
            break;
        case Resource::FONT:
            prefix = "/fonts/";
            break;
        case Resource::VSHADER:
            prefix = "/shaders/vertex/";
            break;
        case Resource::FSHADER:
            prefix = "/shaders/fragment/";
            break;
        case Resource::TEXTURE:
            prefix = "/textures/";
            break;
        case Resource::IMAGE:
            prefix = "/images/";
            break;
        default:
            Logger::Error("LOADER::Can't find such resource type");
            return "";
    }

    return RESOURCE_DIR + prefix + filename;
}
