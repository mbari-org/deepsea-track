cmake_minimum_required(VERSION 3.15)

#######################################
# Project
#######################################
project(deepsea-track VERSION 1.0 DESCRIPTION "MBARI Deep Sea Tracker" LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_SOURCE_DIR src)
set(CMAKE_THIRDPARTY_SOURCE_DIR extern)

#######################################
# Setup auto generation for GIT hash
#######################################
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

#######################################
# Find installed dependencies
#######################################
find_package( Boost 1.72 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
find_package(XercesC 3.2.3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(cppzmq 4.3.3 REQUIRED)

#######################################
# Setup directory configuration
#######################################

# Compiled library code is here
add_subdirectory(src)

# Executable code is here
add_subdirectory(apps)

# Test code is here
add_subdirectory(test)