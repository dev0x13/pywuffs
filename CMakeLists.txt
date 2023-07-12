cmake_minimum_required(VERSION 3.12)

project(pywuffs)

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(UNIX)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -Wall -Wextra -s -fvisibility=hidden -Wl,--strip-all")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/arch:AVX")
endif()

find_package(pybind11 REQUIRED)

pybind11_add_module(pywuffs src/wuffs-bindings.cpp)
target_include_directories(pywuffs PRIVATE libs/wuffs-mirror-release-c/release/)
