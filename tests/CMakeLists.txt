find_package(GTest REQUIRED)
add_executable(weaver_tests test_main.cpp test_memory.cpp test_kv_cache.cpp)
target_link_libraries(weaver_tests PRIVATE weaver_core GTest::gtest_main)
add_test(NAME BasicTest COMMAND weaver_tests)
