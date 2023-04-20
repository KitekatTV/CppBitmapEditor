cmake_minimum_required(VERSION 3.10)
project(CppBitmapEditor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(exe "src/main.cpp")

add_subdirectory("src/lib_bitmap")
target_link_libraries(exe PUBLIC lib_bitmap)

target_include_directories(exe PUBLIC "src/lib_bitmap")
