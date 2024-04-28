cmake_minimum_required(VERSION 3.16)

project(Engine VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

find_package(OpenGL REQUIRED)

add_library(ENGINE STATIC 
            src/engine.cpp
            src/shaders/shaders.cpp
            src/shaders/shader_manager.cpp
            src/model/mesh.cpp
            src/model/model.cpp
            src/input/input.cpp
            src/texture/stb_image.cpp
            src/texture/texture.cpp
            src/logging/logger.cpp
            src/text/text.cpp
            src/text/font.cpp
            src/text/font_manager.cpp
            src/components/animation/animation.cpp
            src/components/animation/skeletal_animations_manager.cpp
            src/components/animation/skeletal_animation_data.cpp
            src/physics/geometry_primitives.cpp
            src/physics/collisions.cpp
            src/components/rigid_body.cpp
            src/components/render_data.cpp
            src/components/collider.cpp
            src/components/transform.cpp
            src/components/camera.cpp 
            src/components/sound.cpp
            src/engine/time.cpp
            src/engine/path_resolver.cpp
            src/engine/math.cpp
            src/object.cpp
            src/images/images.cpp
)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/freetype/freetype)
SET(BUILD_SHARED_LIBS OFF)
SET(ASSIMP_NO_EXPORT ON)
SET(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
SET(ASSIMP_BUILD_OBJ_IMPORTER ON)
SET(ASSIMP_BUILD_COLLADA_IMPORTER ON)
SET(ASSIMP_BUILD_GLTF_IMPORTER ON)
SET(ASSIMP_BUILD_FBX_IMPORTER ON)
SET(ASSIMP_BUILD_GLTF_IMPORTER ON)
SET(ASSIMP_WARNINGS_AS_ERRORS OFF)
SET(ASSIMP_BUILD_SAMPLES OFF)
SET(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(thirdparty/assimp/assimp)

# Profiler intergration
# Uncomment to enable tracy client
# option(TRACY_ENABLE "" ON)
add_subdirectory(thirdparty/tracy)

target_include_directories(ENGINE PUBLIC thirdparty PUBLIC include)
target_include_directories(ENGINE PUBLIC thirdparty/bass/c)

if (UNIX)
    target_link_libraries(ENGINE PUBLIC 
        glfw glad freetype assimp
        ${CMAKE_SOURCE_DIR}/thirdparty/bass/libbass.so
        OpenGL::GL
        Tracy::TracyClient
    )

else()
    target_link_libraries(ENGINE PUBLIC glfw glad freetype assimp 
      ${CMAKE_SOURCE_DIR}/thirdparty/bass/c/x64/bass.lib
      OpenGL::GL)

    add_custom_command (
        TARGET ENGINE
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/bass/x64/bass.dll" 
        "$<TARGET_FILE_DIR:ENGINE>/bass.dll"
    )
endif()

add_executable(test src/main.cpp)
add_executable(game src/game/main.cpp)
target_link_libraries(test PUBLIC ENGINE)
target_link_libraries(game PUBLIC ENGINE)

add_custom_command(TARGET ENGINE PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)

add_custom_command(TARGET ENGINE PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/Debug/resources)


