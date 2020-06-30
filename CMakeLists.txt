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
include_directories (${CMAKE_THIRDPARTY_SOURCE_DIR}/xerces-c/include)
include(GNUInstallDirs)

set(DEEPSEA_CONFIG_INSTALL_DIR        "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE INTERNAL "")
set(DEEPSEA_INCLUDE_INSTALL_DIR       "${CMAKE_INSTALL_INCLUDEDIR}")
set(DEEPSEA_TARGETS_EXPORT_NAME       "${PROJECT_NAME}Targets")
set(DEEPSEA_CMAKE_CONFIG_TEMPLATE     "cmake/config.cmake.in")
set(DEEPSEA_CMAKE_CONFIG_DIR          "${CMAKE_CURRENT_BINARY_DIR}")
set(DEEPSEA_CMAKE_VERSION_CONFIG_FILE "${DEEPSEA_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(DEEPSEA_CMAKE_PROJECT_CONFIG_FILE "${DEEPSEA_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake")
set(DEEPSEA_CMAKE_PROJECT_TARGETS_FILE "${DEEPSEA_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake")


FIND_PACKAGE( Boost 1.72 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# This is unique to Mac OSX - need to refactor
set("OpenCV_DIR" "/usr/local/Cellar/opencv/4.2.0_3")
find_package( OpenCV REQUIRED )

link_directories (${CMAKE_THIRDPARTY_SOURCE_DIR}/xerces-c/lib)
set (XercesLib xerces-c)

add_subdirectory(thirdparty)

#add_executable(test_confignames src/deepsea/util/configclassmap.cc)
#add_executable(test_config src/test/test_config.cc src/deepsea/util/config.cc)
#add_executable(test_flicker src/test/test_flicker.cc src/deepsea/visual_event.cc src/deepsea/image_cache.h src/deepsea/image_cache.h src/deepsea/preprocess.cc src/deepsea/preprocess.h src/deepsea/event_object.cc src/deepsea/event_object.h src/deepsea/util/math_utils.h src/test/test_surprise.cc)
#add_executable(test_surprise src/test/test_surprise.cc src/deepsea/image_cache.h src/deepsea/image_cache.h src/deepsea/preprocess.cc src/deepsea/preprocess.h src/deepsea/event_object.cc src/deepsea/event_object.h src/deepsea/util/math_utils.h src/deepsea/poisson_model.cc src/deepsea/poisson_model.h src/deepsea/surprise_detector.cc src/deepsea/surprise_detector.h src/deepsea/surprise_map.cc src/deepsea/surprise_map.h)
#add_executable(test_preprocess src/test/test_preprocess.cc src/deepsea/visual_event.cc src/deepsea/image_cache.h src/deepsea/image_cache.h src/deepsea/preprocess.cc src/deepsea/preprocess.h src/deepsea/event_object.cc src/deepsea/event_object.h src/deepsea/surprise_detector.cc src/deepsea/surprise_detector.h src/deepsea/poisson_model.h src/deepsea/poisson_model.cc src/deepsea/surprise_map.cc src/deepsea/surprise_map.h)
#add_executable(test_voc src/deepsea/util/voc_object.cc src/deepsea/util/voc_utils.h src/test/test_voc.cpp)
#add_executable(test_eventobject src/test/test_eventobject.cc src/deepsea/util/voc_object.cc src/deepsea/event_object.cc src/deepsea/util/config.cc)
#add_executable(test_visualevent src/deepsea/util/configclassmap.cc src/deepsea/util/config.cc src/deepsea/util/config.h src/deepsea/util/configclassmap.h  src/test/test_visualevent.cc src/deepsea/visual_event.cc src/deepsea/visual_event.h src/deepsea/event_object.cc src/deepsea/surprise_detector.cc src/deepsea/surprise_detector.h src/deepsea/surprise_map.cc src/deepsea/surprise_map.h src/deepsea/poisson_model.h src/deepsea/poisson_model.cc src/deepsea/util/voc_object.cc src/deepsea/util/voc_object.h)
#add_executable(test_tracker src/deepsea/util/voc_object.cc src/deepsea/util/voc_utils.h test/test_tracker.cpp src/deepsea/visual_event.cc src/deepsea/visual_event.h)
#add_executable(test_eventobjecttracker src/deepsea/util/voc_object.cc src/deepsea/util/voc_utils.h src/deepsea/visual_event.cc src/deepsea/event_object.cc src/deepsea/preprocess.cc src/deepsea/image_cache.h src/deepsea/visual_event_mgr.cc src/deepsea/visual_event_mgr.h)
add_executable(deepsea-track src/deepsea/deepsea-track.cc src/deepsea/util/voc_utils.h src/deepsea/util/voc_object.cc
        src/deepsea/util/config.cc src/deepsea/util/config_class_map.cc src/deepsea/visual_event.cc src/deepsea/event_object.cc
        src/deepsea/preprocess.cc src/deepsea/visual_event_mgr.cc src/deepsea/util/utils.cc src/deepsea/util/config_class_map.cc
        src/deepsea/util/config_class_map.h src/deepsea/util/config.cc src/deepsea/util/config.h src/deepsea/logger.cc
        src/deepsea/logger.h src/deepsea/surprise_detector.cc src/deepsea/surprise_detector.h src/deepsea/surprise_map.cc
        src/deepsea/surprise_map.h src/deepsea/poisson_model.h src/deepsea/poisson_model.cc)

# PRIVATE - only affect the current target, not dependencies
target_link_libraries(deepsea-track PRIVATE ${XercesLib} ${OpenCV_LIBS} ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)
#target_link_libraries(test_surprise PRIVATE ${XercesLib} ${OpenCV_LIBS} ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)
#target_link_libraries(test_visualevent PRIVATE ${XercesLib} ${OpenCV_LIBS} ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)
#target_link_libraries(test_eventobject PRIVATE ${XercesLib} ${OpenCV_LIBS} ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)
#target_link_libraries(test_config PRIVATE ${XercesLib} ${OpenCV_LIBS} ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)
#target_link_libraries(test_preprocess PRIVATE ${XercesLib} ${OpenCV_LIBS} ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)