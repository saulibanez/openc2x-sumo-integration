set(gps_SRCS
   GpsService.cpp
)

add_executable (gpsService ${gps_SRCS})
target_include_directories(gpsService PRIVATE ${PROJECT_SOURCE_DIR}/common/interface)

target_link_libraries (gpsService pthread zmq protobuf boost_system boost_thread gps asn proto messages utility uci SumoInterface)

install(TARGETS gpsService DESTINATION bin)
