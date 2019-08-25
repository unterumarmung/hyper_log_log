cmake_minimum_required(VERSION 3.14)
project(hyper_log_log)

set(CMAKE_CXX_STANDARD 11)

add_compile_options (/permissive-)

add_executable(hyper_log_log main.cpp hll/hyper_log_log.hxx hll/murmur_hash.hxx hll/hash.hxx hll/traits.hxx hll/details.hxx hll/helpers.hxx)