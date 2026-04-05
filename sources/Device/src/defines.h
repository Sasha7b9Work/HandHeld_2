// 2024/03/02 09:33:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

// Ручное изменение промежутка между измерениями напряжения
#define NEED_TO_CHANGE_VOLTAGE_MEASUREMENTS

// Отключить контроль напряжения на отладочной плате
//#define POWER_CONTROL_DISABLE

#define VERSION_BUILD 695

#define DATE_BUILD "2026-04-05 17:42:32"

// Врем гашения дисплея после последней кнопки
#define TIME_BLANK_DISPLAY  20000

// Время индикации будильника
#define TIME_SHOW_ALARM     60000

#ifdef GUI
    #define ENABLE_EMULATOR
    #define WIN32_LEAN_AND_MEAN
    #pragma warning(push)
    #pragma warning(disable:4459)
        #include "wx/wx.h"
    #pragma warning(pop)
#endif 


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wmissing-noreturn"
    #pragma clang diagnostic ignored "-Wglobal-constructors"
    #pragma clang diagnostic ignored "-Wpadded"
    #pragma clang diagnostic ignored "-Winvalid-source-encoding"
    #pragma clang diagnostic ignored "-Wc++98-compat"
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
    #pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
    #pragma clang diagnostic ignored "-Wnon-virtual-dtor"
    #pragma clang diagnostic ignored "-Wundefined-func-template"
    #pragma clang diagnostic ignored "-Wdeprecated-register"
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #pragma clang diagnostic ignored "-Wshift-sign-overflow"
    #pragma clang diagnostic ignored "-Wweak-vtables"
#endif


typedef signed char    int8;
typedef unsigned char  uint8;
typedef signed short   int16;
typedef unsigned short uint16;
typedef unsigned int   uint;
typedef const char    *pchar;
typedef unsigned char  uchar;
typedef unsigned long long uint64;


union BitSet16
{
    uint8  b[2];
    uint16 word;
};


#define _bitset(bits)                               \
  ((uint8)(                                         \
  (((uint8)((uint)bits / 01)        % 010) << 0) |  \
  (((uint8)((uint)bits / 010)       % 010) << 1) |  \
  (((uint8)((uint)bits / 0100)      % 010) << 2) |  \
  (((uint8)((uint)bits / 01000)     % 010) << 3) |  \
  (((uint8)((uint)bits / 010000)    % 010) << 4) |  \
  (((uint8)((uint)bits / 0100000)   % 010) << 5) |  \
  (((uint8)((uint)bits / 01000000)  % 010) << 6) |  \
  (((uint8)((uint)bits / 010000000) % 010) << 7)))

#define BINARY_U8( bits ) _bitset(0##bits)

#ifdef WIN32
    #define __asm(x)
    #define __attribute__(x)
#endif

#define _GET_BIT(value, bit) (((value) >> (bit)) & 0x01)
#define _SET_BIT(value, bit) (value |= (1 << (bit)))

#define ENABLED_RU "Вкл"
#define DISABLED_RU "Откл"
