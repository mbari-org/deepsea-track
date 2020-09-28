cmake_minimum_required(VERSION 3.15)

#######################################
# Project
project(deepsea-track VERSION 1.0 DESCRIPTION "MBARI Deep Sea Tracker" LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_SOURCE_DIR src)
set(CMAKE_THIRDPARTY_SOURCE_DIR thirdparty)

#######################################
# Setup auto generation for GIT hash
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get the latest abbreviated commit hash of the working branch
execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions("-DGIT_COMMIT_HASH=${GIT_COMMIT_HASH}")
add_definitions("-DGIT_BRANCH=${GIT_BRANCH}")
configure_file(
        ${CMAKE_SOURCE_DIR}/version.h.in
        ${CMAKE_BINARY_DIR}/generated/version.h
)

#######################################
# Setup directory configuration
include_directories(${CMAKE_SOURCE_DIR}/deepsea)
include_directories(${CMAKE_BINARY_DIR}/generated)
include(GNUInstallDirs)
add_subdirectory(src/deepsea)
add_subdirectory(src/test)