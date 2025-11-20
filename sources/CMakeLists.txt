cmake_minimum_required(VERSION 3.15)

project (HandHeld_2)

set (TARGET_NAME )

set(CMAKE_UNITY_BUILD ON)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_subdirectory(Device)
add_subdirectory(Loader)
add_subdirectory(Transmitter)
