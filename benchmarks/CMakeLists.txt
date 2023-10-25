cmake_minimum_required(VERSION 3.21)

option(SEGMENTS_BUILD_BENCHMARK "Build benchmark for segments" ON)

if (SEGMENTS_BUILD_BENCHMARK)
find_package(benchmark CONFIG REQUIRED)

add_executable(bm_segment bm_segment.cpp)

target_link_libraries(bm_segment PRIVATE segments benchmark::benchmark_main)
endif()
