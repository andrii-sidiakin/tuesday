
function(require_glfw)
    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
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
        GIT_SHALLOW TRUE
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

function(require_glm)
    include(FetchContent)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.1
        OVERRIDE_FIND_PACKAGE
    )
    find_package(glm REQUIRED)

    if (NOT TARGET Tuesday::glm)
        add_library(Tuesday::glm ALIAS glm)
    endif()
endfunction()

function(require_eigen)
    include(FetchContent)
    FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG master # failed on released 3.4.0
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
    find_package(eigen REQUIRED NO_MODULE)
    target_compile_options(eigen INTERFACE -Wno-error)

    if (NOT TARGET Tuesday::eigen)
        add_library(Tuesday::eigen ALIAS eigen)
    endif()
endfunction()

require_glfw()
require_glad()
require_glm()
#require_eigen()

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
    PUBLIC Tuesday::tuesday
        Tuesday::glad_gl
        Tuesday::glm
    PRIVATE Tuesday::glfw
)

target_compile_definitions(eye PUBLIC
    TUE_HAS_GLAD_GL

    TUE_HAS_GLM
    #TUE_HAS_IEGEN3

    #TUE_ASSERT_NOOP
)

