// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PAN3060/PAN3060.h"
#ifdef ENABLE_EMULATOR
    #include "Modules/CMT2210AW/EmulatorReceiver.h"
#endif
#include <gd32e23x.h>
#include <cstring>


namespace PAN3060
{
    static uint64 words[3] = { 0, 0, 0 };
    static uint64 xors[3] = { 0, 0, 0 };

    void AppendBit(bool);

    static void VerifySequence();

    static uint GetBits(uint64);

    static void ExecutePacket(uint); 

    static uint time_enable = 0;        // ¬рем€, когда начались клоки

    static bool need_start = false;
}


void PAN3060::Init()
{
    pinDOUT.Init();

    // »нициализируем пин клоков от приЄмника на прерывание
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    nvic_irq_enable(EXTI4_15_IRQn, 2);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN13);
    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_13);
}


void PAN3060::Update()
{
    if (need_start)
    {
        need_start = false;

        HAL_TIM2::Init();

        time_enable = TIME_MS;
    }
}


void PAN3060::PrepareToSleep()
{
#ifdef WIN32
#else
    EXTI_PD = EXTI_13;
    EXTI_INTEN |= EXTI_13;
#endif
}


void PAN3060::CallbackOnClock()
{
#ifdef WIN32
#else
    EXTI_INTEN &= ~(uint)EXTI_13;
#endif
    need_start = true;
}


bool PAN3060::IsEnabled()
{
    bool result = TIME_MS - time_enable < 700;      // \todo здесь должно быть 610

    if (!result)
    {
        PrepareToSleep();
    }

    return result;
}


void PAN3060::CallbackOnBit()
{
#ifdef ENABLE_EMULATOR

    ExecutePacket(EmuRecv::NextPacket());

#else

    AppendBit(pinDOUT.IsHi());

#endif
}


void PAN3060::AppendBit(bool bit)
{
    words[0] <<= 1;

    if (words[1] & 0x8000000000000000)
    {
        words[0] |= 1;
    }

    words[1] <<= 1;

    if (words[2] & 0x8000000000000000)
    {
        words[1] |= 1;
    }

    words[2] <<= 1;

//    if (EmuRecv::NextBit())
    if (bit)
    {
        words[2] |= 1;
    }

    // ѕосылка будет вот така€ : 1111110 10001101 - 15 бит,
    // где кажда€ единица - это пр€ма€ последовательность баркера(11100010110),
    // а ноль - тоже пр€ма€ последовательность, а не инверсна€ дл€ нул€(00011101001)
    // 0x1c5b8b716e2dc5b8b716e2dc5b8b716e2dc5b8b716

    xors[0] = words[0] ^ 0x0000001C5B8B716E;
    xors[1] = words[1] ^ 0x2DC5B8B716E2DC5B;
    xors[2] = words[2] ^ 0x8B716E2DC5B8B716;

    VerifySequence();
}


uint PAN3060::GetBits(uint64 bits)
{
    static const uint byte_count[256] =
    {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

    uint result = 0;

    while (bits != 0)
    {
        result += byte_count[bits & 0xFF];
        bits >>= 8;
    }

    return result;
}


void PAN3060::VerifySequence()
{
#define BARKERTRESHOLD 3

    uint packet = 0;
    uint bitlevel = 0;

    //check bit HEAD 3
    bitlevel = (uint)(GetBits(xors[0] & 0x0F) + GetBits((xors[1] >> 57) & 0x07FF));
    if (bitlevel < BARKERTRESHOLD)
        packet |= 0x0800;
    else
        if (bitlevel < (11 - BARKERTRESHOLD))
            return;

    //check bit PAYLOAD 5
    bitlevel = (uint)(GetBits((uint)((xors[2] >> 55) & 0x07FF)) + GetBits((uint)(xors[1] & 0x03)));
    if (bitlevel < BARKERTRESHOLD)
        packet |= 0x0020;
    else
        if (bitlevel < (11 - BARKERTRESHOLD))
            return;

    //                           H6      H5      H4      H2     H1     H0     P0   P1   P2   P3   P4    P6    P7
    static const int index[] = { 0,      0,      0,      1,     1,     1,     2,   2,   2,   2,   2,    1,    1 };
    static const int shift[] = { 26,     15,     4,      46,    35,    24,    0,   11,  22,  33,  44,   2,    13 };
    static const uint bit[]  = { 0x4000, 0x2000, 0x1000, 0x400, 0x200, 0x100, 0x1, 0x2, 0x4, 0x8, 0x10, 0x40, 0x80 };

    for (int i = 0; i < 13; i++)
    {
        uint level = GetBits((xors[index[i]] >> shift[i]) & 0x7FF);
        if (level < BARKERTRESHOLD)
        {
            packet |= bit[i];
        }
        else if (level < (11 - BARKERTRESHOLD))
        {
            return;
        }
    }

    ExecutePacket(packet);
}


void PAN3060::ExecutePacket(uint packet)
{
    for (int i = 0; i < Source::Count; i++)
    {
        if (packet == GetCode((Source::E)i))
        {
            Source::Receive((Source::E)i);
            break;
        }
    }
}


uint PAN3060::GetCode(Source::E source)
{
    static const uint packets[Source::Count] = { 0x7E9E, 0x7EA6, 0x7ED5, 0x7EB9, 0x7ECA, 0x7EED };

    return packets[source];
}
