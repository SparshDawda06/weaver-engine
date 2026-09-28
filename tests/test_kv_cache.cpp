// tests/test_kv_cache.cpp
#include <gtest/gtest.h>
#include "weaver/kv_cache.h"
#include "weaver/memory.h"

TEST(KVCacheTest, Initialization) {
    weaver::UnifiedAllocator allocator;
    weaver::H2OCache cache(allocator, 128000, 4096); // 128k total context, 4096 hot slots
    
    EXPECT_EQ(cache.max_capacity(), 128000);
    EXPECT_EQ(cache.hot_capacity(), 4096);
}
