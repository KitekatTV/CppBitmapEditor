cmake_minimum_required(VERSION 3.10)

project(CppBitmapEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(exe "src/app/main.cpp")

add_subdirectory("src/image")
target_link_libraries(exe PUBLIC image)

target_include_directories(exe PUBLIC "src/image")
