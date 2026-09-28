// tests/test_memory.cpp
#include <gtest/gtest.h>
#include "weaver/memory.h"
#include <cuda_runtime.h>

TEST(MemoryTest, PinnedAllocation) {
    weaver::UnifiedAllocator allocator;
    void* ptr = allocator.allocate_pinned_host(1024);
    ASSERT_NE(ptr, nullptr);
    
    // Check if it's actually page-locked
    cudaPointerAttributes attr;
    ASSERT_EQ(cudaPointerGetAttributes(&attr, ptr), cudaSuccess);
    EXPECT_EQ(attr.type, cudaMemoryTypeHost);
    
    allocator.free(ptr);
}

TEST(MemoryTest, DeviceAllocation) {
    weaver::UnifiedAllocator allocator;
    void* ptr = allocator.allocate_device(1024);
    ASSERT_NE(ptr, nullptr);
    
    // Check if it's actually device memory
    cudaPointerAttributes attr;
    ASSERT_EQ(cudaPointerGetAttributes(&attr, ptr), cudaSuccess);
    EXPECT_EQ(attr.type, cudaMemoryTypeDevice);
    
    allocator.free(ptr);
}
