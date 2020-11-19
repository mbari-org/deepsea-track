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
        cppzmq
        deepsea-track_library)

target_link_libraries(deepsea-listen PRIVATE
        ${Boost_LIBRARIES}
        nlohmann_json::nlohmann_json
        ${OpenCV_LIBS}
        ${XercesC_LIBRARIES}
        cppzmq
        deepsea-track_library)