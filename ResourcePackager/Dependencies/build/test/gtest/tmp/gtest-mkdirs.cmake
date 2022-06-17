# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest/src/gtest"
  "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest/src/gtest-build"
  "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest"
  "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest/tmp"
  "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest/src/gtest-stamp"
  "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest/src"
  "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest/src/gtest-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Glizz/Desktop/Programming/Miscellaneous Projects/Console-Game-Engine/ResourcePackager/Dependencies/build/test/gtest/src/gtest-stamp/${subDir}")
endforeach()
