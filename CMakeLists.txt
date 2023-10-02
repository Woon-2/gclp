cmake_minimum_required(VERSION 3.18)

if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(DEFAULT_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")

    message(STATUS "Setting configuration types to ${DEFAULT_CONFIGURATION_TYPES}
        as none was specified."
    )
    set(CMAKE_CONFIGURATION_TYPES ${DEFAULT_CONFIGURATION_TYPES} CACHE STRING
        "Write the possible configuration types."
    )
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(DEFAULT_BUILD_TYPE "Release")
    
    message(STATUS "Settting build type to ${DEFAULT_BUILD_TYPE}
        as none was specified."
    )
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING
        "Choose the type of build."
    )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "${CMAKE_CONFIGURATION_TYPES}"
    )
endif()

if (NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(DEFAULT_INSTALL_PATH "${CMAKE_SOURCE_DIR}/install")

    message(STATUS "Setting install path to ${DEFAULT_INSTALL_PATH}
        as none was specified."
    )
    set (CMAKE_INSTALL_PREFIX "${DEFAULT_INSTALL_PATH}" CACHE STRING
        "Set the install path."
    )
endif()

project(pgclp
    LANGUAGES CXX
    VERSION 2023.10.01
    DESCRIPTION "a lightweight header-only generic command line parser"
)

add_library(gclp INTERFACE cl_parser.hpp)

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}")
    set(DOXYGEN_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/docs")

    if(NOT EXISTS "${DOXYGEN_INPUT_DIR}/docs/Doxyfile")
        configure_file(
            "${DOXYGEN_INPUT_DIR}/Doxyfile.in"
            "${DOXYGEN_OUTPUT_DIR}/Doxyfile"
        )
    endif()

    doxygen_add_docs(doxygen
        "${DOXYGEN_INPUT_DIR}"
        WORKING_DIRECTORY "${DOXYGEN_INPUT_DIR}"
        COMMENT "custom target for automatic documentation"
        CONFIG_FILE "${DOXYGEN_OUTPUT_DIR}/Doxyfile"
    )
endif()

target_compile_features(gclp INTERFACE cxx_std_20)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(test)
endif()