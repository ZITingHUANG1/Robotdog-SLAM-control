set(libsocket_INCLUDE_DIRS "/home/hzt/team26_intro2ros_2023/build/simulation/libsocket-prefix/src/libsocket/headers")

set(libsocket_BINARY_DIR "/home/hzt/team26_intro2ros_2023/build/simulation/libsocket-prefix/src/libsocket-build")

include(${libsocket_BINARY_DIR}/libsocketTargets.cmake)

set(libsocket_LIBRARIES socket++)
