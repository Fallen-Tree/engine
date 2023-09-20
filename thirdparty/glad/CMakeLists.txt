cmake_minimum_required(VERSION 3.16)
project(Glad)

add_compile_options(-std=c++17 -Wall)
add_library(glad include/glad/glad.h src/glad.c)
target_include_directories(glad PUBLIC include/)
