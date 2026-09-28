#include <gtest/gtest.h>
#include "weaver/memory.h"
#include "weaver/kv_cache.h"
#include <vector>

TEST(LongContextTest, Allocate128K) {
    weaver::UnifiedAllocator allocator;
    size_t target_context = 131072; // 128K tokens
    size_t hot_cache = target_context / 20; // 5%
    
    EXPECT_NO_THROW({
        weaver::H2OCache cache(allocator, target_context, hot_cache);
        
        // Simulate adding chunks of 4K tokens up to 128K
        for (size_t i = 0; i < target_context; i += 4096) {
            std::vector<int> dummy_tokens(4096, 1);
            cache.add_tokens(dummy_tokens);
        }
        
        // At this point, eviction must have triggered multiple times
        // We just ensure no crash and proper functioning of the API
        cache.evict_cold_tokens();
    });
}
