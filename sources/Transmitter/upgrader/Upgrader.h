// 2025/11/21 17:00:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <stdint.h>


void upg_init(void);

void upg_update(void);

void upg_begin_update(void);

uint32_t upg_address_begin(void);

int upg_chains_transmitted(void);
int upg_chains_all(void);

// Прошло времени с начала обновления
uint32_t upg_time_passed(void);
