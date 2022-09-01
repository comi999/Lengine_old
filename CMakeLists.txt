#####################################################
# SETUP PROJECT
#####################################################
cmake_minimum_required(VERSION 3.10)
project(Lengine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
file(COPY ${PROJECT_SOURCE_DIR}/Resources DESTINATION ${PROJECT_BINARY_DIR})

#This is not working, but it should be a step after everything is built:
#1. Build the Resource Packager solution into the Example project Resources folder
#2. Run the exe there.
#Note: This will ensure that it's always the updated exe that is produced and used in the Resources folder and that no exe is stored in the repo (yuck)
#execute_process(${PROJECT_BINARY_DIR}/Resources/ResourcePackager.exe)


#####################################################
# SETUP LENGINE-CORE
#####################################################
file(GLOB LENGINE_CORE_SOURCE_FILES ./Lengine-Core/*.cpp)
file(GLOB LENGINE_CORE_HEADER_FILES ./Lengine-Core/*.hpp)
file(GLOB IMGUI_SOURCE_FILES ./Dependencies/imgui/*.cpp)
source_group("Source Files" FILES ${LENGINE_CORE_SOURCE_FILES})
source_group("Header Files" FILES ${LENGINE_CORE_HEADER_FILES})
add_compile_definitions(NOMINMAX)
add_compile_definitions(UNICODE)
option(SOLOUD_BACKEND_SDL2 "" OFF)
option(SOLOUD_BACKEND_XAUDIO2 "" ON)
option(USE_OPENGL "Determines whether or not Lengine will use OpenGL or Console Graphics" ON)
find_package(OpenGL REQUIRED)

if(USE_OPENGL)
	set(RENDERING_BACKEND ./Lengine-Core/Backends/Rendering/Implementation_OpenGL.cpp)
else()
	set(RENDERING_BACKEND ./Lengine-Core/Backends/Rendering/Implementation_ConsoleGL.cpp)
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")
add_library(Lengine-Core STATIC 
	${LENGINE_CORE_SOURCE_FILES}
	${LENGINE_CORE_HEADER_FILES}
	${GLFW_SOURCE_FILES}
	${RENDERING_BACKEND}
	./Dependencies/imgui/backends/imgui_impl_glfw.cpp 
	./Dependencies/imgui/backends/imgui_impl_opengl3.cpp)
add_subdirectory(./Dependencies/soloud/contrib)
add_subdirectory(./Dependencies/glfw)
add_subdirectory(./Dependencies/glew)
target_link_libraries(Lengine-Core PUBLIC
	${OPENGL_LIBRARIES}
	glfw
	libglew_static
	soloud)
target_include_directories(Lengine-Core PUBLIC
	${OPENGL_INCLUDE_DIRS}
	./Dependencies/glfw/include
	./Dependencies/glew/include
	./Dependencies/soloud/include
	./Dependencies/entt/src
	./Dependencies/imgui
	./Dependencies/imgui/backends)


#####################################################
# SETUP LENGINE-RESOURCE-PACKAGER
#####################################################
file(GLOB LENGINE_RESOURCE_PACKAGER_SOURCE_FILES ./Lengine-ResourcePackager/*.cpp)
file(GLOB LENGINE_RESOURCE_PACKAGER_HEADER_FILES ./Lengine-ResourcePackager/*.hpp)
source_group("Source Files" FILES ${LENGINE_RESOURCE_PACKAGER_SOURCE_FILES})
source_group("Header Files" FILES ${LENGINE_RESOURCE_PACKAGER_HEADER_FILES})
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(BUILD_SHARED_LIBS OFF)
add_subdirectory(
	Dependencies/assimp)
add_executable(Lengine-ResourcePackager 
	${LENGINE_RESOURCE_PACKAGER_SOURCE_FILES} 
	${LENGINE_RESOURCE_PACKAGER_HEADER_FILES})
target_link_libraries(Lengine-ResourcePackager PUBLIC 
	Lengine-Core
	assimp)
target_include_directories(Lengine-ResourcePackager PUBLIC
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/Lengine-Core
	Dependencies/assimp/include
	Dependencies/stb/include)


#####################################################
# SETUP LENGINE-EXAMPLE
#####################################################
file(GLOB LENGINE_EXAMPLE_SOURCE_FILES ./Lengine-Example/*.cpp)
file(GLOB LENGINE_EXAMPLE_HEADER_FILES ./Lengine-Example/*.hpp)
source_group("Source Files" FILES ${LENGINE_EXAMPLE_SOURCE_FILES})
source_group("Header Files" FILES ${LENGINE_EXAMPLE_HEADER_FILES})
add_executable(Lengine-Example
	${LENGINE_EXAMPLE_SOURCE_FILES}
	${LENGINE_EXAMPLE_HEADER_FILES})
target_link_libraries(Lengine-Example PUBLIC
	Lengine-Core)
target_include_directories(Lengine-Example PUBLIC
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/Lengine-Core)


#####################################################
# FINAL
#####################################################
if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'Lengine-Example' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Lengine-Example )
    endif()
endif()
