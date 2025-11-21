# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
project (Transmitter)


set(CMAKE_UNITY_BUILD OFF)


include_directories(
    ./vs_keil
    ./core
    ./app
    ./project
    ./radio
    ./upgrader
    ./peripheral
    ./STM32F10x_FWLib/inc
)

add_definitions(-DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(
    /wd4013
    /wd4057
    /wd4100
    /wd4113
    /wd4127    # conditional expression is constant
    /wd4152
    /wd4242
    /wd4244
    /wd4255
    /wd4464    # relative include path contains '..'
    /wd4514    # unreferenced inline function
    /wd4623
    /wd4625
    /wd4626    # assignment operator was implicitly defined as deleted
    /wd4774
    /wd4820    # 'x' bytes padding added after
    /wd4996
    /wd5027    # move assignment operator was implicitly defined as deleted
    /wd5045    # Compiler will insert Spectre mitigation
    /wd5204
    /wd5219
    /wd5220    # a non-static data member with a volatile qualified type no longer implies
)

file(GLOB APP              app/*.*)
file(GLOB PROJECT          project/*.*)
file(GLOB UPGRADER         upgrader/*.*)

source_group(App           FILES ${APP})
source_group(Project       FILES ${PROJECT})
source_group(Upgrader      FILES ${UPGRADER})

add_library(${PROJECT_NAME}
    ${APP}
    ${PROJECT}
    ${UPGRADER}
)
