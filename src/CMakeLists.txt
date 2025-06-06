
function(require_glfw)
    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.4
        OVERRIDE_FIND_PACKAGE
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    find_package(glfw REQUIRED)
    target_compile_options(glfw PRIVATE -Wno-error)

    if (NOT TARGET Tuesday::glfw)
        add_library(Tuesday::glfw ALIAS glfw)
    endif()
endfunction()

#
#
function(require_glad)
    include(FetchContent)
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad/
        GIT_TAG v2.0.8
        OVERRIDE_FIND_PACKAGE
        SOURCE_SUBDIR cmake
    )
    find_package(glad REQUIRED)

    if (NOT TARGET glad_gl_core_4_6)
        glad_add_library(glad_gl_core_4_6 SHARED API gl:core=4.6 EXCLUDE_FROM_ALL)
        target_compile_options(glad_gl_core_4_6 PRIVATE -Wno-error)
    endif()

    if (NOT TARGET glad_vk_1_4)
        glad_add_library(glad_vk_1_4 SHARED LOADER API vulkan=1.4 EXCLUDE_FROM_ALL)
        target_compile_options(glad_vk_1_4 PRIVATE -Wno-error)
    endif()

    if (NOT TARGET Tuesday::glad_gl)
        add_library(Tuesday::glad_gl ALIAS glad_gl_core_4_6)
    endif()
    if (NOT TARGET Tuesday::glad_vk)
        add_library(Tuesday::glad_vk ALIAS glad_vk_1_4)
    endif()

endfunction()

require_glfw()
require_glad()

add_library(eye)
add_library(Tuesday::eye ALIAS eye)

target_include_directories(eye PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_sources(eye PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/window.cpp"
)

target_link_libraries(eye
    PUBLIC Tuesday::tuesday Tuesday::glad_gl
    PRIVATE Tuesday::glfw
)

