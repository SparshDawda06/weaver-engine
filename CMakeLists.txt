cmake_minimum_required(VERSION 3.24)
project(Weaver VERSION 1.0 LANGUAGES CXX)

include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_ARCHITECTURES 89)

add_executable(weaver src/main.cpp)

enable_testing()
add_subdirectory(tests)
