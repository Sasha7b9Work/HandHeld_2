// 2022/10/27 08:02:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifndef WIN32
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
    #pragma clang diagnostic ignored "-Wpadded"
#endif

#if CPU_TYPE == GD32E230
	#include <gd32e23x.h>
#elif CPU_TYPE == GD32F303
	#include <gd32f30x.h>
#endif

#ifndef WIN32
    #pragma clang diagnostic pop
#endif
