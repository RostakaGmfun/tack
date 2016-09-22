add_executable(tack_test main.cpp arp_cache_test.cpp)

target_link_libraries(tack_test tack)

target_include_directories(tack_test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Catch/single_include)

