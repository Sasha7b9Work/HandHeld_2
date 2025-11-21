#include "upgrader/Upgrader.h"

#ifdef WIN32
#define __attribute__()
#endif

__attribute__((used, section(".binary_section")))
const uint8_t binary_data_placeholder[1] = {0xFF};

/*
extern const uint8_t binary_data_start[] __attribute__((section(".binary_section")));
extern const uint8_t binary_data_end[] __attribute__((section(".binary_section")));

#define BINARY_DATA_SIZE (binary_data_end - binary_data_start)
#define DATA_BEGING (&binary_data_start[0])
#define DATA_END (&binary_data_end[0])

static const uint8_t *data = &binary_data_start[0];                   // ”казатель
static const int SIZE_CHAIN = 128;


void upg_data_reset()
{
    data = DATA_BEGING;
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
*/
