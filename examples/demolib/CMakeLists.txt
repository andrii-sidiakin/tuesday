
add_library(demolib)
target_sources(demolib PRIVATE base_app.cpp)
target_include_directories(demolib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(demolib PUBLIC Tuesday::eye)

