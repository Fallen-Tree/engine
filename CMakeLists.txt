cmake_minimum_required(VERSION 3.16)

project(Engine VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include_directories(include)

find_package(OpenGL REQUIRED)

add_library(ENGINE STATIC 
            src/engine.cpp
            src/animation/animation.cpp
            src/shaders/shaders.cpp
            src/model/model.cpp
            src/model/modelLoader.cpp
            src/input/input.cpp
            src/texture/stb_image.cpp
            src/texture/texture.cpp
            src/logging/logger.cpp
            src/geometry_primitives.cpp
            src/collisions.cpp
            src/components/render_data.cpp
            src/components/collider.cpp
            src/components/transform.cpp
            src/components/camera.cpp 
)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)

target_include_directories(ENGINE PUBLIC thirdparty)
target_link_libraries(ENGINE PUBLIC glfw glad OpenGL::GL)

add_executable(main src/main.cpp)
target_link_libraries(main PUBLIC ENGINE)

add_custom_target(RESOURCES ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)

