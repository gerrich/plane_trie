CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

find_package( Boost 1.45.0 COMPONENTS assert)
include_directories( ${Boost_INCLUDE_DIRS} )

include_directories(${CMAKE_INCLUDE_PATH})
LINK_DIRECTORIES(${CMAKE_LIBRARY_PATH})


enable_testing()


add_executable(build_index
  build_index.cpp
)
add_executable(build_trie
  build_trie.cpp
)
add_executable(build_half_trie build_half_trie.cpp)

add_executable(ask_plane_trie ask_plane_trie.cpp)
add_executable(ask_half_trie ask_half_trie.cpp)

add_executable(fuzzy_search_plane_trie fuzzy_search_plane_trie.cpp)
#add_executable(fuzzy_search_half_trie fuzzy_search_half_trie.cpp)

add_executable(fuzzy_intersection_plane_trie
  fuzzy_intersection_plane_trie.cpp
)
add_executable(file_adapter_test
  file_adapter_test.cpp
)

set(TEST_NAME plane_trie_test)
set(TEST_SOURCES ${TEST_NAME}.cpp)
add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} gtest gtest_main pthread )
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

set(TEST_NAME string_less_test)
set(TEST_SOURCES ${TEST_NAME}.cpp)
add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} gtest gtest_main pthread )
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

set(TEST_NAME round_up_test)
set(TEST_SOURCES ${TEST_NAME}.cpp)
add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} gtest gtest_main pthread )
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

set(TEST_NAME istream_adapter_test)
set(TEST_SOURCES ${TEST_NAME}.cpp)
add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} gtest gtest_main pthread )
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

set(TEST_NAME queue_test)
set(TEST_SOURCES ${TEST_NAME}.cpp)
add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME} gtest gtest_main pthread )
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

