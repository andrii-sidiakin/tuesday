cmake_minimum_required(VERSION 3.31)
project(Tuesday VERSION 0.0.0)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

set(CMAKE_CXX_EXTENSIONS OFF)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_library(tuesday INTERFACE)
add_library(Tuesday::tuesday ALIAS tuesday)

target_compile_features(tuesday INTERFACE
    cxx_std_26
)

target_include_directories(tuesday INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_subdirectory(src)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_executable(example0 examples/example0.cpp)
target_link_libraries(example0 PRIVATE Tuesday::tuesday Tuesday::eye)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


