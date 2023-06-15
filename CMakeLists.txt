cmake_minimum_required(VERSION 3.10)
project(CppImageEditor)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()


add_executable(imed "src/app/main.cpp")

include_directories("src/image")
add_subdirectory("src/image")

target_link_libraries(imed PUBLIC image_lib)
target_include_directories(imed PUBLIC "src/image")


add_executable(tests)
target_sources(tests PRIVATE "src/tests/tests.cpp")
target_link_libraries(tests PRIVATE image_lib gtest gtest_main)
target_include_directories(tests PRIVATE "src/image")
gtest_discover_tests(tests)


install(
	TARGETS imed
	EXPORT CppImageEditor_Targets)
