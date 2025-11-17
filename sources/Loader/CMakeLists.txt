# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
project (Loader)


#set(CMAKE_UNITY_BUILD OFF)


include_directories(
    ../common/VS_KEIL
    src/Hardware/HAL
    src
    ../common/GD32E23x_Firmware_Library/Firmware/CMSIS/GD/GD32E23x/Include
    ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Include
)

add_definitions(-DSTM32F407xx)

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

file(GLOB LIBS_HAL         ../common/libs/STM32Cube_F4/Drivers/STM32F4xx_HAL_Driver/Inc/*.*
                           ../common/libs/STM32Cube_F4/Drivers/STM32F4xx_HAL_Driver/Src/*.*)
file(GLOB SRC              src/*.*)
file(GLOB HARDWARE         src/Hardware/*.*)
file(GLOB HARDWARE_CMSIS   src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL     src/Hardware/HAL/*.*)
file(GLOB MODULES_PAN3060  src/Modules/PAN3060/*.*)
file(GLOB UTILS            src/Utils/*.*)

set_source_files_properties(${LIBS_HAL} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group(src             FILES ${SRC})
source_group(libs/HAL        FILES ${LIBS_HAL})
source_group(Hardware        FILES ${HARDWARE})
source_group(Hardware/CMSIS  FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL    FILES ${HARDWARE_HAL})
source_group(Modules/PAN3060 FILES ${MODULES_PAN3060})
source_group(Utils           FILES ${UTILS})

add_library(
    ${PROJECT_NAME}
    ${LIBS_HAL}
    ${SRC}
    ${HARDWARE}
    ${HARDWARE_CMSIS}
    ${HARDWARE_HAL}
    ${MODULES_PAN3060}
    ${UTILS}
)
