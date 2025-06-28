tue_require_glfw()
tue_require_glad()
tue_require_glm()

add_library(eye)
add_library(Tuesday::eye ALIAS eye)

target_include_directories(eye PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_sources(eye PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp"
)

target_link_libraries(eye
    PUBLIC Tuesday::tuesday
        Tuesday::glad_gl
        Tuesday::glm
    PRIVATE Tuesday::glfw
)

target_compile_definitions(eye PUBLIC
    TUE_HAS_GLAD_GL
    TUE_HAS_GLM
)

