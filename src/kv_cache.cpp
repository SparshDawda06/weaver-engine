// src/kv_cache.cpp
#include "weaver/kv_cache.h"

namespace weaver {

H2OCache::H2OCache(UnifiedAllocator& allocator, size_t max_cap, size_t hot_cap)
    : allocator_(allocator), max_capacity_(max_cap), hot_capacity_(hot_cap) {}

size_t H2OCache::max_capacity() const { return max_capacity_; }
size_t H2OCache::hot_capacity() const { return hot_capacity_; }

}
