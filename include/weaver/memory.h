// include/weaver/memory.h
#pragma once
#include <cstddef>

namespace weaver {
class UnifiedAllocator {
public:
    void* allocate_device(size_t size);
    void* allocate_pinned_host(size_t size);
    void free(void* ptr);
};
}
