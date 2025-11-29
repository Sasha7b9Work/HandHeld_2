// 2025/11/22 12:20:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <stdint.h>


static const int TIM_PACKETS = 0;           // Ётот таймер контролирует промежутки между пакетами
static const int TIM_ELAPSED_UPGRADE = 1;   // —только времени прошло с начала обновлени€
static const int TIM_TX = 2;

#define TIME_MS Timer_CurrentTime()

uint32_t Timer_CurrentTime(void);

void Timer_Reset(int num_timer);

uint32_t Timer_ElapsedMS(int num_timer);

void Timer_DelayMS(uint32_t);

extern uint32_t counter_ms;

