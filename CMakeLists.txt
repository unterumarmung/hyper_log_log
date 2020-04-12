cmake_minimum_required(VERSION 3.14)
project(hyper_log_log)

set(CMAKE_CXX_STANDARD 11)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    add_compile_options(-Wall -pedantic-errors)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    add_compile_options(/Zc:__cplusplus /permissive- /Wall)
endif()


add_executable(hyper_log_log main.cpp hll/hyper_log_log.hxx hll/murmur_hash.hxx hll/hash.hxx hll/traits.hxx hll/details.hxx hll/helpers.hxx)