cmake_minimum_required(VERSION 3.10)

project(CppBitmapEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(imed "src/app/main.cpp")

include_directories("src/image")
add_subdirectory("src/image")

target_link_libraries(imed PUBLIC image_lib)
target_include_directories(imed PUBLIC "src/image")
