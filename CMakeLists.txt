cmake_minimum_required(VERSION 3.16)

project(Engine VERSION 0.1)

add_compile_options(-std=c++17 -Wall)
set(CMAKE_BUILD_TYPE Debug)

include_directories(include)			# Расположение заголовочных файлов

find_package(OpenGL REQUIRED)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_subdirectory(src/model)

add_executable(main src/main.cpp)

target_link_libraries(main PUBLIC glfw glad OpenGL::GL model)