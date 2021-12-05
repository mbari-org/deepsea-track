cmake_minimum_required(VERSION 3.15)

#######################################
# Project
#######################################
project(deepsea-track VERSION 1.0 DESCRIPTION "MBARI Deep Sea Tracker" LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_SOURCE_DIR src)
set(CMAKE_THIRDPARTY_SOURCE_DIR extern)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -O0 -fno-omit-frame-pointer")
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
find_package( Boost COMPONENTS program_options filesystem REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
find_package(XercesC 3 REQUIRED)
find_package(OpenCV 4.5 REQUIRED)
find_package(nlohmann_json 3 REQUIRED)
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
