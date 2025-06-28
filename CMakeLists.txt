cmake_minimum_required(VERSION 3.31)
project(Tuesday VERSION 0.0.0)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

option(TUE_BUILD_TESTS "Build tests" YES)
option(TUE_BUILD_EXAMPLES "Build examples" YES)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(tue_utils)

set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_library(tuesday INTERFACE)
add_library(Tuesday::tuesday ALIAS tuesday)

target_compile_features(tuesday INTERFACE
    cxx_std_23
)

target_compile_options(tuesday INTERFACE
    -Wall -Wpedantic -Wextra
)

target_include_directories(tuesday INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)

target_compile_definitions(tuesday INTERFACE
    #TUE_ASSERT_NOOP
)

target_compile_definitions(tuesday INTERFACE
    _GLIBCXX_DEBUG
    _GLIBCXX_DEBUG_BACKTRACE
)
target_link_libraries(tuesday INTERFACE
    stdc++exp
)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_subdirectory(src)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

if(TUE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

if(TUE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


