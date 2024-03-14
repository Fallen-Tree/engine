cmake_minimum_required(VERSION 3.5)

project(Engine VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
            src/images/images.cpp
)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/freetype/freetype)

target_include_directories(ENGINE PUBLIC thirdparty PUBLIC include)
target_include_directories(ENGINE PUBLIC thirdparty/irrKlang/include)

if (UNIX)
    target_link_libraries(ENGINE PUBLIC 
        glfw glad freetype 
        ${CMAKE_SOURCE_DIR}/thirdparty/irrKlang/libIrrKlang.so 
        ${CMAKE_SOURCE_DIR}/thirdparty/irrKlang/ikpMP3.so 
        OpenGL::GL)

    add_custom_command (
        TARGET ENGINE
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/irrKlang/ikpMP3.so" 
        "$<TARGET_FILE_DIR:ENGINE>/ikpMP3.so"
    )

else()
    target_link_libraries(ENGINE PUBLIC glfw glad freetype ${CMAKE_SOURCE_DIR}/thirdparty/irrKlang/irrKlang.lib OpenGL::GL)

    add_custom_command(TARGET ENGINE PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/Debug/resources)

    add_custom_command(TARGET ENGINE PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)

    add_custom_command (
        TARGET ENGINE
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/irrKlang/irrKlang.dll" 
        "$<TARGET_FILE_DIR:ENGINE>/irrKlang.dll"
    )

    add_custom_command (
        TARGET ENGINE
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/irrKlang/ikpMP3.dll" 
        "$<TARGET_FILE_DIR:ENGINE>/ikpMP3.dll"
    )
endif()

add_executable(main src/main.cpp)
target_link_libraries(main PUBLIC ENGINE)

add_custom_target(RESOURCES ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)
