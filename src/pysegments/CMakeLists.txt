cmake_minimum_required(VERSION 3.21)



set_target_properties(segments PROPERTIES POSITION_INDEPENDENT_CODE ON)

pybind11_add_module(pysegments MODULE
        pysegments.cpp
        pysegments.h
        )
target_link_libraries(pysegments PRIVATE
        segments
        pybind11::pybind11)

set_target_properties(pysegments PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/pysegments
        LIBRARY_OUTPUT_NAME _segments)

install(TARGETS pysegments
        LIBRARY DESTINATION pysegments
        RUNTIME DESTINATION pysegments
        )
