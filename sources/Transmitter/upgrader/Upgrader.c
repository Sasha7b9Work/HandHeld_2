#include "upgrader/Upgrader.h"

#ifdef WIN32
#define __attribute__()
#endif

__attribute__((used, section(".binary_section")))
const uint8_t binary_data_placeholder[1] = {0xFF};      // Этот байт будет размещён в первом байте .binary_section.
                                                        // Т.е. после заливки прошивки нужно перепрошивать двоичные данные

#define DATA_BEGIN ((uint8_t *)0x08030000)
#define DATA_SIZE 0xd800
#define DATA_END (DATA_BEGIN + DATA_SIZE)

static const uint8_t *data = (const uint8_t *)DATA_BEGIN;
static const int SIZE_CHAIN = 128;


// Сбрасывает указатель данных прошивки
static void upg_data_reset(void);

// Возвращает указатель на следующую порцию данных. size - размер порции
static const uint8_t *upg_data_next(int *size);


void upg_update()
{

}


void upg_data_reset()
{
    data = DATA_BEGIN;
}


const uint8_t *upg_data_next(int *size)
{
    const uint8_t *result = data;

    if (DATA_END - result >= SIZE_CHAIN)
    {
        *size = SIZE_CHAIN;
    }
    else
    {
        *size = DATA_END - result;
    }

    data += *size;

    return result;
}
