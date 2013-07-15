CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

enable_testing()

add_executable(plane_trie_test
  plane_trie_test.cpp
)

add_executable(string_less_test
  string_less_test.cpp)
target_link_libraries(string_less_test ${TEST_SOURCES} gtest gtest_main pthread )
add_test(NAME string_less_test COMMAND string_less_test)

add_executable(round_up_test
  round_up_test.cpp)
target_link_libraries(round_up_test ${TEST_SOURCES} gtest gtest_main pthread )
add_test(NAME round_up_test COMMAND round_up_test)
