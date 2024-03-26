cmake_minimum_required(VERSION 3.5)

project(Engine VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

find_package(OpenGL REQUIRED)

add_library(ENGINE STATIC 
            src/engine.cpp
            src/animation/animation.cpp
            src/shaders/shaders.cpp
            src/model/mesh.cpp
            src/model/model.cpp
            src/input/input.cpp
            src/texture/stb_image.cpp
            src/texture/texture.cpp
            src/logging/logger.cpp
            src/text/text.cpp
            src/text/font.cpp
            src/animation/animation.cpp
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
SET(ASSIMP_WARNINGS_AS_ERRORS OFF)
SET(ASSIMP_BUILD_SAMPLES OFF)
SET(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(thirdparty/assimp/assimp)

target_include_directories(ENGINE PUBLIC thirdparty PUBLIC include)
target_include_directories(ENGINE PUBLIC thirdparty/bass/c)


if (UNIX)
    add_custom_target(RESOURCES ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)

    target_link_libraries(ENGINE PUBLIC 
        glfw glad freetype assimp
        ${CMAKE_SOURCE_DIR}/thirdparty/bass/libbass.so
        OpenGL::GL
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

    add_custom_command(
        TARGET ENGINE PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
    )
    
endif()

add_executable(main src/main.cpp)
target_link_libraries(main PUBLIC ENGINE)

