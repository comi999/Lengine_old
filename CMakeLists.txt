cmake_minimum_required(VERSION 3.10)

project(CGE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(CGE)
add_subdirectory(ResourcePackager)
add_subdirectory(TestProject)

file(COPY ${PROJECT_SOURCE_DIR}/TestProject/Resources DESTINATION ${PROJECT_BINARY_DIR}/TestProject/)

