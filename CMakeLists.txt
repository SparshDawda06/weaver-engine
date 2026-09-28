cmake_minimum_required(VERSION 3.24)
project(Weaver VERSION 1.0 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 89)

find_package(CUDAToolkit REQUIRED)

add_library(weaver_core STATIC src/memory.cpp src/kv_cache.cpp)
target_include_directories(weaver_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(weaver_core PUBLIC CUDA::cudart)

add_executable(weaver src/main.cpp)
target_link_libraries(weaver PRIVATE weaver_core)

enable_testing()
add_subdirectory(tests)
