cmake_minimum_required(VERSION 3.21)

option(SEGMENTS_BUILD_TESTS "Build tests for segments" ON)

add_library(segments STATIC
        segments.h
        segment.cpp
        expanding_searcher.cpp
        expanding_searcher.h
)


target_include_directories(segments PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
        )


if (SEGMENTS_BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    find_package(Boost REQUIRED)
    include(GoogleTest)

    add_executable(test_segments
            test_search.cpp)
    target_link_libraries(test_segments PRIVATE
            GTest::gtest_main
            Boost::boost
            segments)

    gtest_discover_tests(test_segments)

endif()
