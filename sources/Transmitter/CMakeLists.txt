# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
project (Transmitter)


set(CMAKE_UNITY_BUILD OFF)


include_directories(
    .
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
SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -MP -Wall")

add_compile_options(
    /wd4002
    /wd4068    # unknown pragma 'xxx'
    /wd4113
    /wd4273    # 'xxx' : inconsistent dll linkage
)

file(GLOB APP              app/*.*)
file(GLOB PERIPHERAL       peripheral/*.*)
file(GLOB PROJECT          project/*.*)
file(GLOB RADIO            radio/*.*)
file(GLOB UPGRADER         upgrader/*.*)

source_group(App           FILES ${APP})
source_group(Peripheral    FILES ${PERIPHERAL})
source_group(Project       FILES ${PROJECT})
source_group(Radio         FILES ${RADIO})
source_group(Upgrader      FILES ${UPGRADER})

add_library(${PROJECT_NAME}
    ${APP}
    ${PERIPHERAL}
    ${PROJECT}
    ${RADIO}
    ${UPGRADER}
)
