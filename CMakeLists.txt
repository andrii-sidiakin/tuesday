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

target_compile_options(tuesday INTERFACE
    -Wall -Wpedantic -Wextra
)

target_include_directories(tuesday INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_subdirectory(src)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

add_executable(example_wsi examples/example_wsi.cpp)
target_link_libraries(example_wsi PRIVATE Tuesday::tuesday Tuesday::eye)

add_executable(example_sg examples/example_sg.cpp)
target_link_libraries(example_sg PRIVATE Tuesday::tuesday Tuesday::eye)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


