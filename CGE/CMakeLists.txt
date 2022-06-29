file(GLOB CGE_SOURCES ./*.cpp)
file(GLOB CGE_HEADERS ./*.hpp)

source_group("Sources" FILES ${CGE_SOURCES})
source_group("Headers" FILES ${CGE_HEADERS})

option(SOLOUD_BACKEND_SDL2 OFF)
option(SOLOUD_BACKEND_XAUDIO2 ON)
add_subdirectory(Dependencies/soloud/contrib)

add_library(CGE STATIC ${CGE_SOURCES} ${CGE_HEADERS})
target_link_libraries(CGE PUBLIC soloud)

target_include_directories(CGE PUBLIC Dependencies/soloud/include)

