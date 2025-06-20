
function(tue_add_simple_test NAME)
    cmake_parse_arguments(a "" "GROUP" "SOURCES;LIBRARIES" ${ARGN})

    if(NOT DEFINED a_GROUP)
        set(a_GROUP tue)
    endif()

    set(t_fullname ${a_GROUP}_${NAME})
    set(t_target test_${t_fullname})

    if(NOT DEFINED a_SOURCES)
        set(a_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cpp)
        message(CHECK_START "Looking for '${a_SOURCES}'")
        if(NOT EXISTS ${a_SOURCES})
            message(CHECK_FAIL "Cannot auto-discover source file '${a_SOURCES}'. Skipping test [${t_fullname}]...")
            return()
        else()
            message(CHECK_PASS)
        endif()
    endif()

    if(NOT DEFINED a_LIBRARIES)
        set(a_LIBRARIES Tuesday::tuesday)
    endif()

    add_executable(${t_target} ${a_SOURCES})

    if(DEFINED a_LIBRARIES)
        target_link_libraries(${t_target} PRIVATE ${a_LIBRARIES})
    endif()

    add_test(NAME ${t_fullname} COMMAND ${t_target})
endfunction()

tue_add_simple_test(tseq GROUP mp)
