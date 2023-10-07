function(copy_doxyrm INPUT_SOURCES OUTPUT_SOURCES)
    find_package(Python COMPONENTS Interpreter)

    if(Python_FOUND)
        set(Doxyrm_PATH "${PROJECT_SOURCE_DIR}/extern/doxyrm" CACHE PATH "Path to doxyrm script")

        foreach(INPUT_FILE "${INPUT_SOURCES}")
            list(FIND INPUT_SOURCES "${INPUT_FILE}" INDEX)
            list(GET OUTPUT_SOURCES ${INDEX} OUTPUT_FILE)

            add_custom_command(
                OUTPUT "${OUTPUT_FILE}"
                COMMAND "${Python_EXECUTABLE}" "${Doxyrm_PATH}/doxyrm.py"
                    -i "${INPUT_FILE}"
                    -o "${OUTPUT_FILE}"
                DEPENDS "${INPUT_FILE}"
            )

            list(APPEND DOXYRM_OUTPUT_FILES "${OUTPUT_FILE}")
        endforeach()

        add_custom_target(doxyrm ALL
            COMMENT "custom target for generating code copy without doxygen comments"
            DEPENDS "${DOXYRM_OUTPUT_FILES}"
        )
    endif()
endfunction()

macro(doxygen_option OPTION_NAME DESCRIPTION DEFAULT)
    option("${OPTION_NAME}_CONFIG" "${DESCRIPTION}" ${DEFAULT})
    if("${OPTION_NAME}_CONFIG")
        set("${OPTION_NAME}" YES)
    else()
        set("${OPTION_NAME}" NO)
    endif()
endmacro()

function(add_doxygen DOXYGEN_INPUT_DIR DOXYGEN_OUTPUT_DIR)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        return()
    endif()

    set(Doxygen_ROOT "" CACHE PATH
        "The path to directory where doxygen is installed
        used in find_package(Doxygen),
        This path is used prior to any other paths."
    )

    doxygen_option(DOXYGEN_RECURSIVE "doxygen recursive option" OFF)

    doxygen_option(DOXYGEN_EXTRACT_ALL
        "extract all entities including even enitites not documented.
        (turn this option on when you need develop-documentation.)"
        OFF
    )

    option(DOXYGEN_EXCLUDE_DETAIL 
        "ignore clp::detail namespace when documenting with doxygen.
        (turn this option off when you need develop-documentation.)"
        ON
    )

    if (DOXYGEN_EXCLUDE_DETAIL)
        list(APPEND DOXYGEN_PREDEFINITIONS
            "DOXYGEN_IGNORE_DETAIL"
        )
    endif()

    option(BUILD_DOXYGEN
        "enable if you want to document with doxygen 
        each time you build the project."
        OFF
    )

    if(BUILD_DOXYGEN)
        set(DOXYGEN_ALL ALL)
    endif()

    configure_file("${DOXYGEN_INPUT_DIR}/Doxyfile.in"
        "${DOXYGEN_OUTPUT_DIR}/Doxyfile"
        @ONLY
    )

    doxygen_add_docs(doxygen
        "${DOXYGEN_INPUT_DIR}"
        WORKING_DIRECTORY "${DOXYGEN_INPUT_DIR}"
        ${DOXYGEN_ALL}
        COMMENT "custom target for automatic documentation"
        CONFIG_FILE "${DOXYGEN_OUTPUT_DIR}/Doxyfile"
    )
endfunction()