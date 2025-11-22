// 2025/11/22 12:20:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <stdint.h>


static const int TIM_PACKETS = 0;       // Этот таймер контролирует промежутки между пакетами

#define TIME_MS Timer_CurrentTime()

static uint32_t Timer_CurrentTime(void);

static void Timer_Reset(int num_timer);
static uint32_t Timer_ElapsedMS(int num_timer);


extern uint32_t counter_ms;
