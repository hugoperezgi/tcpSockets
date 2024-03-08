cmake_minimum_required(VERSION 3.0)
project(SyssockTCPSockets)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


set(SOURCES
    main.cpp
)

add_executable(SyssockTCPSockets ${SOURCES})
