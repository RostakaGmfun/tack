cmake_minimum_required(VERSION 3.0.2)

project(tack LANGUAGES CXX)

find_package(Doxygen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

option(BUILD_FOR_TRAVIS "Build for Travis-CI" OFF)

include(Modules/Doxygen.cmake)

set(TACK_SOURCES
    src/network_device.cpp
    src/ndev_pool.cpp
    src/ndev_worker.cpp
    src/ethernet.cpp
    src/arp.cpp
    src/arp_cache.cpp
    src/sockbuf.cpp
    src/ipv4.cpp)

add_library(tack ${TACK_SOURCES})
target_include_directories(tack PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/Catch/single_include)
target_link_libraries(tack pthread)

# IFF_MULTI_QUEUE is not defined in linux kernel headers on travis-ci VM
if (BUILD_FOR_TRAVIS)
    message(STATUS "Building for Travis")
    target_compile_definitions(tack PRIVATE -DIFF_MULTI_QUEUE=0)
endif (BUILD_FOR_TRAVIS)

add_subdirectory(test)
add_subdirectory(examples)
