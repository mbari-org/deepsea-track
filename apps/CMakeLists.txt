add_executable(deepsea-track deepsea-track.cc)
target_compile_features(deepsea-track PRIVATE cxx_std_17)
include_directories(${CMAKE_SOURCE_DIR}/include)
configure_file(
        ${CMAKE_SOURCE_DIR}/src/version.h.in
        ${CMAKE_SOURCE_DIR}/include/deepsea/version.h
)
target_link_libraries(deepsea-track PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json
        ${OpenCV_LIBS}
        ${XercesC_LIBRARIES}
        deepsea-track_library)

#
#add_executable(deepsea-track deepsea-track.cc util/voc_utils.h util/voc_object.cc
#        util/config.cc util/config_class_map.cc visual_event.cc event_object.cc
#        preprocess.cc visual_event_mgr.cc util/utils.cc util/config_class_map.cc
#        util/config_class_map.h util/config.cc util/config.h logger.cc
#        logger.h arguments.cc arguments.h surprise_detector.cc surprise_detector.h
#        surprise_map.cc surprise_map.h poisson_model.h poisson_model.cc)