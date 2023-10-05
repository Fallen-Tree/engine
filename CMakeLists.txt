cmake_minimum_required(VERSION 3.16)

project(Engine VERSION 0.1)

add_compile_options(-std=c++17 -Wall)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

 include_directories(src/model)			# Расположение заголовочных файлов

find_package(OpenGL REQUIRED)

add_library(MODEL STATIC 
            src/model/model.cpp 
            src/model/model.hpp
            src/model/modelInstance.cpp
            src/model/modelInstance.hpp)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)

add_executable(main src/main.cpp)

target_include_directories(main PUBLIC thirdparty)
target_link_libraries(main PUBLIC glfw glad OpenGL::GL MODEL)
