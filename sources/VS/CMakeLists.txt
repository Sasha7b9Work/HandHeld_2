# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

cmake_minimum_required(VERSION 3.5)

# Set project name
project (HandHels-GUI)

set (TARGET_NAME HandHels-GUI)

set(CMAKE_UNITY_BUILD ON)

#if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
#    add_definitions(-DDEBUG)
#endif()

add_subdirectory(Device)
