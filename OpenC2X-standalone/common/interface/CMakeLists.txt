set(SUMO_HOME "/home/saul/Descargas/sumo-1.23.1")

add_library(SumoInterface
    SumoInterface.cpp
    SumoInterface.h
    ${SUMO_HOME}/src/utils/traci/TraCIAPI.cpp
    ${SUMO_HOME}/src/foreign/tcpip/socket.cpp
    ${SUMO_HOME}/src/foreign/tcpip/storage.cpp
)

target_include_directories(SumoInterface
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${SUMO_HOME}/src
        ${SUMO_HOME}/src/foreign
        ${SUMO_HOME}/src/utils/traci
        ${SUMO_HOME}/src/libsumo
)
