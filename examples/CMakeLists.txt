add_subdirectory(demolib)
add_subdirectory(demo1)

add_executable(example_wsi example_wsi.cpp)
target_link_libraries(example_wsi PRIVATE Tuesday::tuesday Tuesday::eye)

add_executable(example_gfx example_gfx.cpp)
target_link_libraries(example_gfx PRIVATE Tuesday::tuesday Tuesday::eye demolib)

