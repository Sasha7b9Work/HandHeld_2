# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

#set(CMAKE_UNITY_BUILD OFF)

message("------------------------------------------------------------------------ ${PROJECT_NAME}")

include_directories(
    ../common/VS_KEIL
    src/Hardware/HAL
    src
    ../common/GD32E23x_Firmware_Library/Firmware/CMSIS/GD/GD32E23x/Include
    ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Include
)

add_definitions(-DGD32E230 -DTYPE_BOARD_799)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(
    /wd4310
    /wd4514
    /wd4623
    /wd4626
    /wd4710
    /wd4820    # 'xxx': '4' bytes padding added after data member 'yyy'
    /wd4996
    /wd5027
    /wd5045
    /wd5204
)

file(GLOB GD32E23x                ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Include/*.*
                                  ../common/GD32E23x_Firmware_Library/Firmware/GD32E23x_standard_peripheral/Source/*.*)
file(GLOB SRC                     src/*.*)
file(GLOB DISPLAY                 src/Display/Display7735.*
                                  src/Display/Icons.*
                                  src/Display/Colors.*)
file(GLOB DISPLAY_FONT            src/Display/Font/*.*)
file(GLOB HARDWARE                src/Hardware/*.*)
file(GLOB HARDWARE_CMSIS          src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL            src/Hardware/HAL/*.*)
file(GLOB KEYBOARD                src/Keyboard/*.*)
file(GLOB MENU                    src/Menu/*.*)
file(GLOB MENU_PAGES              src/Menu/Pages/*.*)
file(GLOB MODULES_BEEPER          src/Modules/Beeper/*.*)
file(GLOB MODULES_BEEPER_SOUNDS   src/Modules/Beeper/Sounds/*.*)
file(GLOB MODULES_CMT2210AW       src/Modules/CMT2210AW/*.*)
file(GLOB MODULES_LED             src/Modules/LED/*.h
                                  src/Modules/LED/LED.cpp
                                  src/Modules/LED/driverLED_7735.cpp)
file(GLOB MODULES_PCF8563         src/Modules/PCF8563/*.*)
file(GLOB MODULES_ST7735          src/Modules/ST7735/*.*)
file(GLOB SETTINGS                src/Settings/*.*)
file(GLOB STORAGE                 src/Storage/*.*)
file(GLOB UTILS                   src/Utils/*.*)

source_group(GD32E23x               FILES ${GD32E23x})
source_group(src                    FILES ${SRC})
source_group(Display                FILES ${DISPLAY})
source_group(Display/Font           FILES ${DISPLAY_FONT})
source_group(Hardware               FILES ${HARDWARE})
source_group(Hardware/CMSIS         FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL           FILES ${HARDWARE_HAL})
source_group(Keyboard               FILES ${KEYBOARD})
source_group(Menu                   FILES ${MENU})
source_group(Menu/Pages             FILES ${MENU_PAGES})
source_group(Modules/Beeper         FILES ${MODULES_BEEPER})
source_group(Modules/Beeper/Sounds  FILES ${MODULES_BEEPER_SOUNDS})
source_group(Modules/CMT2210AW      FILES ${MODULES_CMT2210AW})
source_group(Modules/LED            FILES ${MODULES_LED})
source_group(Modules/PCF8563        FILES ${MODULES_PCF8563})
source_group(Modules/ST7735         FILES ${MODULES_ST7735})
source_group(Settings               FILES ${SETTINGS})
source_group(Storage                FILES ${STORAGE})
source_group(Utils                  FILES ${UTILS})

set_source_files_properties(
    ${GD32E23x}
    ${HARDWARE_CMSIS}
    src/Hardware/Watch.cpp
    PROPERTIES HEADER_FILE_ONLY TRUE
)

add_library(${PROJECT_NAME}
    ${GD32E23x}
    ${SRC}
    ${DISPLAY}
    ${DISPLAY_FONT}
    ${HARDWARE}
    ${HARDWARE_CMSIS}
    ${HARDWARE_HAL}
    ${MENU}
    ${MENU_PAGES}
    ${KEYBOARD}
    ${MODULES_BEEPER}
    ${MODULES_BEEPER_SOUNDS}
    ${MODULES_CMT2210AW}
    ${MODULES_LED}
    ${MODULES_PCF8563}
    ${MODULES_ST7735}
    ${SETTINGS}
    ${STORAGE}
    ${UTILS}
)
