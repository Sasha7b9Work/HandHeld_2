// 2025/11/21 17:00:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <stdint.h>


void upg54_init(void);

void upg54_update(void);

// Вызывается передатчиком после передачи пакета
void upg54_on_tx_irq(void);

// Запуск передачи. num_KB - столько килбоайт передавать
void upg54_start_update(int num_KB);

void upg54_stop_update(void);

uint32_t upg54_address_begin(void);

void upg54_func_display(void);
