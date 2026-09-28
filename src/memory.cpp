// src/memory.cpp
#include "weaver/memory.h"
#include <cuda_runtime.h>
#include <stdexcept>
#include <string>
#include <iostream>

namespace weaver {

void* UnifiedAllocator::allocate_device(size_t size) {
    void* ptr = nullptr;
    cudaError_t err = cudaMalloc(&ptr, size);
    if (err != cudaSuccess) {
        throw std::runtime_error(std::string("CUDA malloc failed: ") + cudaGetErrorString(err));
    }
    return ptr;
}

void* UnifiedAllocator::allocate_pinned_host(size_t size) {
    void* ptr = nullptr;
    cudaError_t err = cudaMallocHost(&ptr, size);
    if (err != cudaSuccess) {
        throw std::runtime_error(std::string("CUDA malloc host failed: ") + cudaGetErrorString(err));
    }
    return ptr;
}

void UnifiedAllocator::free(void* ptr) noexcept {
    if (!ptr) return;
    cudaPointerAttributes attr;
    cudaError_t err = cudaPointerGetAttributes(&attr, ptr);
    if (err != cudaSuccess) {
        std::cerr << "CUDA pointer get attributes failed: " << cudaGetErrorString(err) << std::endl;
        return;
    }
    
    if (attr.type == cudaMemoryTypeHost) {
        err = cudaFreeHost(ptr);
    } else {
        err = cudaFree(ptr);
    }
    
    if (err != cudaSuccess) {
        std::cerr << "CUDA free failed: " << cudaGetErrorString(err) << std::endl;
    }
}
}
