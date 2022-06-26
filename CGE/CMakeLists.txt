cmake_minimum_required(VERSION 3.10)

project(CGE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB CGE_SOURCES ./*.cpp)
file(GLOB CGE_HEADERS ./*.hpp)

source_group("Sources" FILES ${CGE_SOURCES})
source_group("Headers" FILES ${CGE_HEADERS})

add_library(CGE STATIC ${CGE_SOURCES} ${CGE_HEADERS})

