# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
project (Transmitter)


set(CMAKE_UNITY_BUILD OFF)


include_directories(
    ./vs_keil
    ./core
    ./app
    ./project
    ./upgrader
    ./peripheral
    ./STM32F10x_FWLib/inc
)

add_definitions(-DSTM32F10X_HD)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(
    /wd4127    # conditional expression is constant
    /wd4464    # relative include path contains '..'
    /wd4514    # unreferenced inline function
    /wd4623
    /wd4625
    /wd4626    # assignment operator was implicitly defined as deleted
    /wd4820    # 'x' bytes padding added after
    /wd4996
    /wd5027    # move assignment operator was implicitly defined as deleted
    /wd5045    # Compiler will insert Spectre mitigation
    /wd5204
    /wd5219
    /wd5220    # a non-static data member with a volatile qualified type no longer implies
)

file(GLOB UPGRADER         upgrader/*.*)
file(GLOB PROJECT          project/*.*)

source_group(Upgrader      FILES ${UPGRADER})
source_group(Project       FILES ${PROJECT})

add_library(
    ${PROJECT_NAME}
    ${PROJECT}
    ${UPGRADER}
)
