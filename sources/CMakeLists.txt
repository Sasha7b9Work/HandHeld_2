cmake_minimum_required(VERSION 3.15)

project (HandHeld_2)

set(CMAKE_UNITY_BUILD ON)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_subdirectory(Device/vs7735)
add_subdirectory(Device/vs7789)
add_subdirectory(Loader)
add_subdirectory(Transmitter)
