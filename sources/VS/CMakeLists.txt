# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

cmake_minimum_required(VERSION 3.15)

# Set project name
project (HandHels-GUI)

set (TARGET_NAME HandHels-GUI)

set(CMAKE_UNITY_BUILD ON)

add_subdirectory(Device7735)
add_subdirectory(Device7789)
