# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

#set(CMAKE_UNITY_BUILD OFF)

set(CMAKE_UNITY_BUILD_BATCH_SIZE 16)


include_directories(
    .
    Device
    ../../Device/src
    ../../Device/src/Hardware/HAL
    ../../VS/ThirdParty/wxWidgets/include
    ../../generated/ThirdParty/lib/vc_dll/mswud
    ../../common/VS_KEIL
)

link_directories(../../generated/ThirdParty/lib/vc_dll)

add_definitions(-DSTM32F103xB -DWXUSINGDLL -DGUI -DDEVICE)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

    add_compile_options(
        /wd4355
        /wd4365
        /wd4505
        /wd4514
        /wd4623
        /wd4625
        /wd4626
        /wd4710
        /wd4774
        /wd4820
        /wd5026
        /wd5027
        /wd5039    # 'xxx': pointer or reference to potentially throwing function passed to extern C
                                    # function under -EHc.
        /wd5045
        /wd5204
        /wd5267
    )
endif()

file(GLOB GUI                                   GUI/*.*)
file(GLOB GUI_CONTROLS                          GUI/Controls/*.*)
file(GLOB SRC                                   *.h *.cpp
                                   ../../Device/src/Device.*
                                   ../../Device/src/*.h)
file(GLOB DISPLAY                  ../../Device/src/Display/Colors.*
                                   ../../Device/src/Display/Display7735.*
                                   ../../Device/src/Display/Icons.*)
file(GLOB DISPLAY_FONT             ../../Device/src/Display/Font/*.*)
file(GLOB HARDWARE                 ../../Device/src/Hardware/Power.h
                                   ../../Device/src/Hardware/Power.*
                                   ../../Device/src/Hardware/Watch.*
                                   ../../Device/src/Hardware/Timer.h
                                   ../../Device/src/Hardware/Vibrato.h
                                                    Hardware/*.*)
file(GLOB HARDWARE_HAL             ../../Device/src/Hardware/HAL/HAL.h
                                                    Hardware/HAL/*.*)
file(GLOB KEYBOARD                 ../../Device/src/Keyboard/Keyboard.h
                                                    Keyboard/*.*)
file(GLOB MODULES_BEEPER           ../../Device/src/Modules/Beeper/*.h
                                   ../../Device/src/Modules/Beeper/Beeper.cpp
                                                    Modules/Beeper/*.*)
file(GLOB MODULES_BEEPER_SOUNDS    ../../Device/src/Modules/Beeper/Sounds/*.*)
file(GLOB MODULES_CMT2210AW        ../../Device/src/Modules/CMT2210AW/*.*
                                                    Modules/CMT2210AW/*.*)
file(GLOB MODULES_LED              ../../Device/src/Modules/LED/LED.*
                                                    Modules/LED/*.*)
file(GLOB MODULES_PCF8563          ../../Device/src/Modules/PCF8563/PCF8563.*
                                                    Modules/PCF8563/*.*)
file(GLOB MODULES_ST7735           ../../Device/src/Modules/ST7735/*.h)
file(GLOB MENU                     ../../Device/src/Menu/*.*)
file(GLOB MENU_PAGES               ../../Device/src/Menu/Pages/*.*)
file(GLOB SETTINGS                 ../../Device/src/Settings/*.*)
file(GLOB STORAGE                  ../../Device/src/Storage/*.*)
file(GLOB UTILS                    ../../Device/src/Utils/*.*)

source_group(GUI                   FILES ${GUI})
source_group(GUI/Controls          FILES ${GUI_CONTROLS})
source_group(src                   FILES ${SRC})
source_group(Display               FILES ${DISPLAY})
source_group(Display/Font          FILES ${DISPLAY_FONT})
source_group(Hardware              FILES ${HARDWARE})
source_group(Hardware/HAL          FILES ${HARDWARE_HAL})
source_group(Keyboard              FILES ${KEYBOARD})
source_group(Modules/Beeper        FILES ${MODULES_BEEPER})
source_group(Modules/Beeper/Sounds FILES ${MODULES_BEEPER_SOUNDS})
source_group(Modules/CMT2210AW     FILES ${MODULES_CMT2210AW})
source_group(Modules/LED           FILES ${MODULES_LED})
source_group(Modules/PCF8563       FILES ${MODULES_PCF8563})
source_group(Modules/ST7735        FILES ${MODULES_ST7735})
source_group(Menu                  FILES ${MENU})
source_group(Menu/Pages            FILES ${MENU_PAGES})
source_group(Settings              FILES ${SETTINGS})
source_group(Storage               FILES ${STORAGE})
source_group(Utils                 FILES ${UTILS})


add_executable(${PROJECT_NAME}
    ${GUI}
    ${GUI_CONTROLS}
    ${SRC}
    ${DISPLAY}
    ${DISPLAY_FONT}
    ${HARDWARE}
    ${HARDWARE_HAL}
    ${KEYBOARD}
    ${MODULES_BEEPER}
    ${MODULES_BEEPER_SOUNDS}
    ${MODULES_CMT2210AW}
    ${MODULES_LED}
    ${MODULES_PCF8563}
    ${MODULES_ST7735}
    ${MENU}
    ${MENU_PAGES}
    ${SETTINGS}
    ${STORAGE}
    ${UTILS}
)

target_link_libraries(${PROJECT_NAME} wxbase33ud wxmsw33ud_core Ws2_32)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxbase331ud_vc_custom.dll $(OutputPath) /Y)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxmsw331ud_core_vc_custom.dll $(OutputPath) /Y)
