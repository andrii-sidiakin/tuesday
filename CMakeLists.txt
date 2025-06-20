cmake_minimum_required(VERSION 3.31)
project(Tuesday VERSION 0.0.0)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_subdirectory(src)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_subdirectory(examples)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


