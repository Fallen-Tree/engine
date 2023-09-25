cmake_minimum_required(VERSION 3.16)

project(model VERSION 0.1)

add_compile_options(-std=c++17 -Wall)

project(model)				# Название проекта

set(SOURCE_LIB model.cpp)		# Установка переменной со списком исходников

add_library(model STATIC ${SOURCE_LIB})# Создание статической библиотеки



