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

    add_custom_target(doc ALL DEPENDS ${DOXYGEN_OUTPUT})
else ()
    message(WARNING "Doxygen not found, documentation can not be generated")
endif (DOXYGEN_FOUND)
