include(FetchContent)

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

function(tue_require_glfw)
    if(TARGET Tuesday::glfw)
        return()
    endif()

    FetchContent_Declare(glfw
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

    add_library(Tuesday::glfw ALIAS glfw)

endfunction()

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

function(tue_require_glad)
    if(NOT TARGET glad)
        FetchContent_Declare(glad
            GIT_REPOSITORY https://github.com/Dav1dde/glad/
            GIT_TAG v2.0.8
            GIT_SHALLOW TRUE
            OVERRIDE_FIND_PACKAGE
            SOURCE_SUBDIR cmake
        )
        find_package(glad REQUIRED)
    endif()

    if (NOT TARGET glad_gl_core_4_6)
        glad_add_library(glad_gl_core_4_6 SHARED API gl:core=4.6 EXCLUDE_FROM_ALL)
        target_compile_options(glad_gl_core_4_6 PRIVATE -Wno-error)
    endif()

    if (NOT TARGET glad_vk_1_4)
        glad_add_library(glad_vk_1_4 SHARED LOADER API vulkan=1.4 EXCLUDE_FROM_ALL)
        target_compile_options(glad_vk_1_4 PRIVATE -Wno-error)
    endif()

    add_library(Tuesday::glad_gl ALIAS glad_gl_core_4_6)
    add_library(Tuesday::glad_vk ALIAS glad_vk_1_4)

    add_library(tue_glad INTERFACE)
    target_link_libraries(tue_glad INTERFACE Tuesday::glad_gl Tuesday::glad_vk)
    add_library(Tuesday::glad ALIAS tue_glad)

endfunction()

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

function(tue_require_glm)
    if (TARGET Tuesday::glm)
        return()
    endif()

    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )

    find_package(glm REQUIRED)

    add_library(Tuesday::glm ALIAS glm)

endfunction()

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

function(tue_require_doctest)
    if (TARGET Tuesday::doctest)
        return()
    endif()

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )

    find_package(doctest REQUIRED)

    add_library(Tuesday::doctest ALIAS doctest)

endfunction()

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


function(tue_require_nanobench)
    if (TARGET Tuesday::nanobench)
        return()
    endif()

    FetchContent_Declare(
        nanobench
        GIT_REPOSITORY https://github.com/martinus/nanobench.git
        GIT_TAG v4.3.11
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )

    find_package(nanobench REQUIRED)

    add_library(Tuesday::nanobench ALIAS nanobench)

endfunction()

## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

