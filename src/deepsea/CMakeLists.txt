# Find installed dependencies
find_package( Boost 1.72 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
find_package(XercesC 3.2.3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(deepsea-track deepsea-track.cc util/voc_utils.h util/voc_object.cc
        util/config.cc util/config_class_map.cc visual_event.cc event_object.cc
        preprocess.cc visual_event_mgr.cc util/utils.cc util/config_class_map.cc
        util/config_class_map.h util/config.cc util/config.h logger.cc
        logger.h surprise_detector.cc surprise_detector.h surprise_map.cc
        surprise_map.h poisson_model.h poisson_model.cc)

target_link_libraries(deepsea-track ${Boost_LIBRARIES} nlohmann_json::nlohmann_json ${OpenCV_LIBS} ${XercesC_LIBRARIES})
