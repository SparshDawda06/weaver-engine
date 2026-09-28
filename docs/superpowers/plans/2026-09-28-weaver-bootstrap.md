# Weaver Engine Bootstrap Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Bootstrap the Weaver C++/CUDA inference engine by implementing the core build system, unified memory allocator, and the H2O KV Cache interface.

**Architecture:** A C++20 project built with CMake, targeting CUDA 12.x. The unified allocator manages page-locked host memory and device memory to prevent OS out-of-memory errors. The KV cache interface defines the Heavy Hitter (H2O) eviction API.

**Tech Stack:** C++20, CUDA, CMake, GoogleTest

## Global Constraints

- Must build with CMake 3.24+ and GCC 11+ / NVCC 12.x
- C++20 standard strictly enforced
- No Python dependencies for the core runtime
- CUDA kernels must support compute capability 8.9 (RTX 40/50 series)

---

### Task 1: CMake Build System and Project Skeleton

**Files:**
- Create: `CMakeLists.txt`
- Create: `tests/CMakeLists.txt`
- Create: `src/main.cpp`
- Create: `tests/test_main.cpp`

**Interfaces:**
- Consumes: N/A
- Produces: An executable `weaver` and a test executable `weaver_tests`

- [ ] **Step 1: Write the failing test**

```cpp
// tests/test_main.cpp
#include <gtest/gtest.h>

TEST(WeaverBootstrap, BasicMath) {
    EXPECT_EQ(2 + 2, 4);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `mkdir build && cd build && cmake .. && make && ./tests/weaver_tests`
Expected: FAIL with CMake errors (file not found).

- [ ] **Step 3: Write minimal implementation**

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.24)
project(Weaver VERSION 1.0 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_ARCHITECTURES 89)

add_executable(weaver src/main.cpp)

enable_testing()
add_subdirectory(tests)
```

```cmake
# tests/CMakeLists.txt
find_package(GTest REQUIRED)
add_executable(weaver_tests test_main.cpp)
target_link_libraries(weaver_tests GTest::gtest_main)
add_test(NAME BasicTest COMMAND weaver_tests)
```

```cpp
# src/main.cpp
#include <iostream>

int main() {
    std::cout << "Weaver Engine initialized." << std::endl;
    return 0;
}
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd build && cmake .. && make && ./tests/weaver_tests`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add CMakeLists.txt tests/CMakeLists.txt src/main.cpp tests/test_main.cpp
git commit -m "chore: setup CMake build system and test harness"
```

### Task 2: Unified Memory Allocator (CUDA)

**Files:**
- Create: `include/weaver/memory.h`
- Create: `src/memory.cpp`
- Create: `tests/test_memory.cpp`

**Interfaces:**
- Consumes: Task 1 build system
- Produces: `weaver::UnifiedAllocator` class with `allocate_device`, `allocate_pinned_host`, and `free` methods.

- [ ] **Step 1: Write the failing test**

```cpp
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
    cudaPointerGetAttributes(&attr, ptr);
    EXPECT_EQ(attr.type, cudaMemoryTypeHost);
    
    allocator.free(ptr);
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd build && make && ./tests/weaver_tests`
Expected: FAIL with "weaver/memory.h: No such file or directory"

- [ ] **Step 3: Write minimal implementation**

```cpp
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
```

```cpp
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
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd build && make && ./tests/weaver_tests`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add include/weaver/memory.h src/memory.cpp tests/test_memory.cpp
git commit -m "feat: add unified CUDA memory allocator"
```

### Task 3: H2O KV Cache Interface

**Files:**
- Create: `include/weaver/kv_cache.h`
- Create: `src/kv_cache.cpp`
- Create: `tests/test_kv_cache.cpp`

**Interfaces:**
- Consumes: `weaver::UnifiedAllocator`
- Produces: `weaver::H2OCache` class with `add_tokens` and `evict_cold_tokens` methods.

- [ ] **Step 1: Write the failing test**

```cpp
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
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cd build && make && ./tests/weaver_tests`
Expected: FAIL with missing file.

- [ ] **Step 3: Write minimal implementation**

```cpp
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
};
}
```

```cpp
// src/kv_cache.cpp
#include "weaver/kv_cache.h"

namespace weaver {

H2OCache::H2OCache(UnifiedAllocator& allocator, size_t max_cap, size_t hot_cap)
    : allocator_(allocator), max_capacity_(max_cap), hot_capacity_(hot_cap) {}

size_t H2OCache::max_capacity() const { return max_capacity_; }
size_t H2OCache::hot_capacity() const { return hot_capacity_; }

}
```

- [ ] **Step 4: Run test to verify it passes**

Run: `cd build && make && ./tests/weaver_tests`
Expected: PASS

- [ ] **Step 5: Commit**

```bash
git add include/weaver/kv_cache.h src/kv_cache.cpp tests/test_kv_cache.cpp
git commit -m "feat: add H2O KV cache interface skeleton"
```
