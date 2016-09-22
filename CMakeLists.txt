cmake_minimum_required(VERSION 3.0.2)

project(tack LANGUAGES CXX)

find_package(Doxygen)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

include(Modules/Doxygen.cmake)

set(TACK_SOURCES
    src/network_device.cpp
    src/ndev_pool.cpp
    src/ethernet.cpp
    src/arp.cpp
    src/arp_cache.cpp)

add_library(tack ${TACK_SOURCES})
target_include_directories(tack PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(tack pthread)

add_executable(tackdev src/main.cpp)
target_link_libraries(tackdev tack)


add_subdirectory(test)
