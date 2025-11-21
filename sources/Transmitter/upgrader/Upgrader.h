// 2025/11/21 17:00:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <stdint.h>


// Сбрасывает указатель данных прошивки
void upg_data_reset(void);

// Возвращает указатель на следующую порцию данных. size - размер порции
const uint8_t *upg_data_next(int *size);
