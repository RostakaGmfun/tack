cmake_minimum_required(VERSION 3.0.2)

project(tack CXX)

set(TACK_SOURCES
    src/main.cpp
    src/network_device.cpp
    src/device_pool.cpp
    )

add_executable(tack ${TACK_SOURCES})

target_include_directories(tack PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(tack pthread)

set_target_properties(tack PROPERTIES CXX_STANDARD 11)
