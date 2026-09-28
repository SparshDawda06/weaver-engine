// src/memory.cpp
#include "weaver/memory.h"
#include <cuda_runtime.h>
#include <stdexcept>

namespace weaver {

void* UnifiedAllocator::allocate_device(size_t size) {
    void* ptr = nullptr;
    if (cudaMalloc(&ptr, size) != cudaSuccess) {
        throw std::runtime_error("CUDA malloc failed");
    }
    return ptr;
}

void* UnifiedAllocator::allocate_pinned_host(size_t size) {
    void* ptr = nullptr;
    if (cudaMallocHost(&ptr, size) != cudaSuccess) {
        throw std::runtime_error("CUDA malloc host failed");
    }
    return ptr;
}

void UnifiedAllocator::free(void* ptr) {
    if (!ptr) return;
    cudaPointerAttributes attr;
    if (cudaPointerGetAttributes(&attr, ptr) == cudaSuccess) {
        if (attr.type == cudaMemoryTypeHost) {
            cudaFreeHost(ptr);
        } else {
            cudaFree(ptr);
        }
    }
}
}
