// 2024/03/23 13:22:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifdef GUI
    #define ENABLE_EMULATOR
#endif

// Воспроизводить звуки
#define SOUND_ENABELD

// В рабочей прошивке нужно раскомментировать, чтобы была обработка напряжения аккумулятора
// При отладке нужно отключать, чтобы не мешало работе с отладчиком (выводит НИЗКОЕ НАПРЯЖЕНИЕ)
#define POWER_CONTROL_ENABLED
