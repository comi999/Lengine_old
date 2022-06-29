file(GLOB RESOURCE_PACKAGER_SOURCES ./*.cpp)
file(GLOB RESOURCE_PACKAGER_HEADERS ./*.hpp)

source_group("Sources" FILES ${RESOURCE_PACKAGER_SOURCES})
source_group("Headers" FILES ${RESOURCE_PACKAGER_HEADERS})

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(BUILD_SHARED_LIBS OFF)
add_subdirectory(Dependencies/assimp)

include_directories(Dependencies/assimp/include)

add_executable(ResourcePackager ${RESOURCE_PACKAGER_SOURCES} ${RESOURCE_PACKAGER_HEADERS})
target_link_libraries(ResourcePackager PUBLIC CGE assimp)

target_include_directories(ResourcePackager PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/CGE"
    )

