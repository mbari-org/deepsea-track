file(GLOB HEADER_LIST CONFIGURE_DEPENDS
        "${deepsea-track_SOURCE_DIR}/include/deepsea/*.h"
        "${deepsea-track_SOURCE_DIR}/include/deepsea/util/*.h")

add_library(deepsea-track_library arguments.cc
        event_object.cc logger.cc poisson_model.cc preprocess.cc
        surprise_detector.cc surprise_map.cc visual_event.cc
        visual_event_mgr.cc zmq_listener.cc util/config.cc util/config_class_map.cc
        util/utils.cc util/voc_object.cc ${HEADER_LIST})

target_include_directories(deepsea-track_library PUBLIC ../include)

target_link_libraries(deepsea-track_library PRIVATE
        ${XercesLib}
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json)

target_compile_features(deepsea-track_library PUBLIC cxx_std_11)
