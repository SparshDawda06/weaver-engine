cmake_minimum_required(VERSION 3.24)
project(Weaver VERSION 1.0 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 89)

add_executable(weaver src/main.cpp src/memory.cpp)
find_package(CUDAToolkit REQUIRED)
target_include_directories(weaver PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(weaver PRIVATE CUDA::cudart)

enable_testing()
add_subdirectory(tests)
