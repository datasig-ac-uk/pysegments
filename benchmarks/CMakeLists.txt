cmake_minimum_required(VERSION 3.21)


find_package(benchmark CONFIG REQUIRED)



add_executable(bm_segment bm_segment.cpp)

target_link_libraries(bm_segment PRIVATE segments benchmark::benchmark_main)

target_compile_options(bm_segment PRIVATE -g -pg -fno-omit-frame-pointer)
