cmake_minimum_required(VERSION 4.0)
project(zmq_simple_pirate_pattern)

set(CMAKE_CXX_STANDARD 17)

find_library(LIB_CZMQ czmq HINTS /usr/lib/x86_64-linux-gnu)

if(LIB_CZMQ)
    message(STATUS "Found CZMQ library: ${LIB_CZMQ}")
else()
    message(FATAL_ERROR "CZMQ library not found!")
endif()

add_executable(spqueue spqueue.cpp
        zmsg.h zhelpers.h)
target_link_libraries(spqueue ${LIB_CZMQ} zmq)

add_executable(spworker spworker.cpp zmsg.h zhelpers.h)
target_link_libraries(spworker ${LIB_CZMQ} zmq)

add_executable(lpclient lpclient.cpp lpclient.h)
target_link_libraries(lpclient zmq)
