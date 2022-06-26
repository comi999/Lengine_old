file(GLOB CGE_SOURCES ./*.cpp)
file(GLOB CGE_HEADERS ./*.hpp)

source_group("Sources" FILES ${CGE_SOURCES})
source_group("Headers" FILES ${CGE_HEADERS})

add_library(CGE STATIC ${CGE_SOURCES} ${CGE_HEADERS})

