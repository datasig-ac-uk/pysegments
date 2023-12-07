cmake_minimum_required(VERSION 3.21)
project(segments)

option(SEGMENTS_PYTHON_MODULE "Build the Python interface" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)


enable_testing()
add_subdirectory(src/segments)

if (SEGMENTS_PYTHON_MODULE)
    set(PYBIND11_FINDPYTHON ON)
    find_package(Python COMPONENTS Interpreter Development.Module)
    add_subdirectory(extern/pybind)
    add_subdirectory(src/pysegments)
endif()


add_subdirectory(benchmarks)
