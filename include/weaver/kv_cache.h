#include <vector>
// include/weaver/kv_cache.h
#pragma once
#include "weaver/memory.h"
#include <cstdint>

namespace weaver {
class H2OCache {
private:
    UnifiedAllocator& allocator_;
    size_t max_capacity_;
    size_t hot_capacity_;
    
public:
    H2OCache(UnifiedAllocator& allocator, size_t max_cap, size_t hot_cap);
    
    size_t max_capacity() const;
    size_t hot_capacity() const;
    
    void add_tokens(const std::vector<int>& tokens);
    void evict_cold_tokens();
};
}
