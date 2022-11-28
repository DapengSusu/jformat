cmake_minimum_required(VERSION 3.16)
project(format VERSION 0.1.1)

# Debug or Release
SET(CMAKE_BUILD_TYPE "Debug")

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    fmt_test
    fmt_test.cpp
)

target_link_libraries(
    fmt_test
    GTest::gtest_main
)

add_executable(
    format_test
    format_test.cpp
)

target_link_libraries(
    format_test
    GTest::gtest_main
)

add_executable(
    logtracer_test
    logtracer.cpp
    logtracer_test.cpp
)

include(GoogleTest)
gtest_discover_tests(fmt_test)
gtest_discover_tests(format_test)
