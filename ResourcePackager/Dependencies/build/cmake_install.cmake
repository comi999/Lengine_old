# Install script for directory: C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/assimp-3.3.1/assimp-3.3.1

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp3.3.1-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-3.3" TYPE FILE FILES
    "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/assimp-config.cmake"
    "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/assimp-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp3.3.1-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/assimp.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/contrib/zlib/cmake_install.cmake")
  include("C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/code/cmake_install.cmake")
  include("C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/tools/assimp_cmd/cmake_install.cmake")
  include("C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
