cmake_minimum_required(VERSION 3.0.2)

project(tack CXX)

find_package(Doxygen)

set(DOC_DIR ${CMAKE_BINARY_DIR}/doc/ CACHE PATH "Documentation directory")
set(DOXYGEN_OUTPUT ${DOC_DIR}/html/index.html)
set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

if (DOXYGEN_FOUND)
    add_custom_command(
        OUTPUT ${DOXYGEN_OUTPUT}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${DOXYGEN_INPUT}
    )
else ()
    message(WARNING "Doxygen not found, documentation can not be generated")
endif (DOXYGEN_FOUND)

add_custom_target(doc ALL DEPENDS ${DOXYGEN_OUTPUT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

set(TACK_SOURCES
    src/main.cpp
    src/network_device.cpp
    src/ndev_pool.cpp
    src/ethernet.cpp
    src/arp.cpp
    )

add_executable(tack ${TACK_SOURCES})

target_include_directories(tack PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(tack pthread)

set_target_properties(tack PROPERTIES CXX_STANDARD 11)
