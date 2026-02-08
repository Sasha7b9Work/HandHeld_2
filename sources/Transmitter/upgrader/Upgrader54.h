// 2025/11/21 17:00:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <stdint.h>


void upg_init(void);

void upg_update(void);

// Вызывается передатчиком после передачи пакета
void upg_on_tx_irq(void);

// Запуск передачи. num_KB - столько килбоайт передавать
void upg_start_update(int num_KB);

void upg_stop_update(void);

uint32_t upg_address_begin(void);

void upg_func_display(void);
