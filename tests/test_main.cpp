// tests/test_main.cpp
#include <gtest/gtest.h>

TEST(WeaverBootstrap, BasicMath) {
    EXPECT_EQ(2 + 2, 4);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
