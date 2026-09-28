find_package(GTest REQUIRED)
add_executable(weaver_tests test_main.cpp)
target_link_libraries(weaver_tests GTest::gtest_main)
add_test(NAME BasicTest COMMAND weaver_tests)
