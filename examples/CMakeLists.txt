
add_executable(example_wsi example_wsi.cpp)
target_link_libraries(example_wsi PRIVATE Tuesday::tuesday Tuesday::eye)

add_executable(example_sg example_sg.cpp)
target_link_libraries(example_sg PRIVATE Tuesday::tuesday Tuesday::eye)

add_executable(example_gfx example_gfx.cpp)
target_link_libraries(example_gfx PRIVATE Tuesday::tuesday Tuesday::eye demolib)

