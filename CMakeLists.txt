cmake_minimum_required(VERSION 3.16)
project(format VERSION 0.2.0)

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
    tests/fmt_test.cpp
)

target_link_libraries(
    fmt_test
    GTest::gtest_main
)

target_include_directories(fmt_test
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

add_executable(
    format_test
    tests/format_test.cpp
)

target_link_libraries(
    format_test
    GTest::gtest_main
)

target_include_directories(format_test
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

add_executable(
    logtracer_test
    src/logtracer.cpp
    tests/logtracer_test.cpp
)

target_include_directories(logtracer_test
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(fmt_test)
gtest_discover_tests(format_test)
