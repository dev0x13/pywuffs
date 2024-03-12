cmake_minimum_required(VERSION 3.12)

project(pywuffs)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS
      ${CMAKE_CXX_FLAGS}
      "-std=c++14 -Wall -Wextra -s -fvisibility=hidden -Wl,--strip-all")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif(MSVC)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/arch:AVX")
endif()

file(
  DOWNLOAD
  https://raw.githubusercontent.com/google/wuffs/13c72db3508d33b9416a22a0ab8a8d4d8d5cd7be/release/c/wuffs-unsupported-snapshot.c
  ${CMAKE_SOURCE_DIR}/src/wuffs-unsupported-snapshot.c)

find_package(pybind11 REQUIRED)

pybind11_add_module(pywuffs src/wuffs-bindings.cpp)
