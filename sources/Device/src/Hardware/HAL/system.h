#pragma once

#ifdef MODEL7789

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
    #pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

    #include "gd32f30x.h"
#endif

#ifdef MODEL7735
    #include "gd32e23x.h"
#endif
