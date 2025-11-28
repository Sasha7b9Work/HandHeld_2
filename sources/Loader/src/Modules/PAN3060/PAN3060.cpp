// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/PAN3060/chirp_rf.h"
#include "Display/Text.h"
#include "Hardware/Timer.h"
#include "Utils/String.h"
#include <gd32e23x.h>


namespace PAN3060
{
    static bool need_recv = false;

    static bool in_process_upgrade = false;

    static void InitIRQ();

    static void InitSPI();

    //    static const uint begin_firmware = 0x08030000;
    static const uint size_firmware = 54 * 1024;
    static const uint SIZE_CHAIN = 128;
    static const int chains_in_page = 8;

    // Здесь хранятся контрольные суммы для каждого пакета
    static const uint num_chanins = size_firmware / SIZE_CHAIN;
    static uint crc[num_chanins];

    static uint8 page[1024];            // Здесь будет принятая страница, котору мы целиком запишем в память


    static int current_chain = 0;               // Данный чайн сейчас будет приниматься.
    static int received_chains_in_page = 0;     // Столько чайнов принято на данной странице. Если != 0, то надо записывать в память
    static int chains_is_ok = 0;
    static int chains_is_fail = 0;

    static void Reset();

    static void ReceiveChainPacket(uint8 buffer[256]);
    static void ReceiveFinishPacket(uint8 buffer[256]);
}


void PAN3060::Init()
{
    pinSPI1_NSS.Init();
    pinSPI1_NSS.ToHi();

    InitIRQ();

    InitSPI();

    rf_init();

    rf_set_default_para();

    rf_enter_continous_rx();

    for (uint i = 0; i < num_chanins; i++)
    {
        crc[i] = 0;
    }

    Reset();
}


void PAN3060::Reset()
{
    current_chain = 0;
    received_chains_in_page = 0;
    chains_is_ok = 0;
    chains_is_fail = 0;
}


void PAN3060::InitIRQ()
{
    // Инициализируем пин клоков от приёмника на прерывание
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    nvic_irq_enable(EXTI4_15_IRQn, 2);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN8);
    exti_init(EXTI_8, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_8);
}


void PAN3060::InitSPI()
{
    pinSPI1_MOSI.Init();
    pinSPI_MISO.Init();
    pinSPI1_CLK.Init();

    pinSPI1_MOSI.ToLow();
    pinSPI1_CLK.ToLow();
}


void PAN3060::Update()
{
    if (need_recv)
    {
        need_recv = false;

        in_process_upgrade = true;

        uint8 irq = rf_read_spec_page_reg(PAGE0_SEL, 0x6C);

        if (irq & REG_IRQ_RX_TIMEOUT)
        {
            rf_clr_irq();
        }

        if (irq & REG_IRQ_RX_DONE)
        {
            uint8 buffer[256];

            uint8 len = rf_read_spec_page_reg(PAGE1_SEL, 0x7D);

            rf_read_fifo(REG_FIFO_ACC_ADDR, buffer, len);

            rf_clr_irq();

            if (len == 2 + 128 + 4)            // Принимаем 128 байт прошивки
            {
                ReceiveChainPacket(buffer);

                rf_init();
                rf_set_default_para();
                rf_enter_continous_rx();
            }
            else if (len == 2 + 4 + 4)         // Принимаем завершающий пакет
            {
                ReceiveFinishPacket(buffer);

                rf_init();
                rf_set_default_para();
                rf_enter_continous_rx();
            }
        }
    }
}


void PAN3060::ReceiveChainPacket(uint8 buffer[256])
{
    if ((current_chain % chains_in_page) == 0)
    {
        // Заполняем все биты единицами - запись такого буфера в ПЗУ никак его не изменит.
        // Поэтому можно будет записывать всё целиком, даже если нужно записать только один chain
        std::memset(page, 0xFF, 1024);
    }

    Struct16 number_chain(buffer);

    if (crc[number_chain.u16] == 0)      // Этот чайн ещё не принят - нет контрольной суммы
    {
        Struct32 crc_recv(buffer + 2 + 128);

        uint crc_calc = SU::CalculateCRC32(buffer, 2 + 128);

        if (crc_calc == crc_recv.u32)
        {
            chains_is_ok++;

            crc[number_chain.u16] = crc_calc;

            std::memcpy(page + (number_chain.u16 % chains_in_page) * SIZE_CHAIN, buffer + 2, SIZE_CHAIN);
        }
        else
        {
            chains_is_fail++;
        }
    }

}


void PAN3060::ReceiveFinishPacket(uint8 [256])
{

}


void PAN3060::CallbackOnIRQ()
{
    need_recv = true;
}


bool PAN3060::InProcessUpgrade()
{
    return in_process_upgrade;
}


void PAN3060::FuncDraw()
{
    Text<>("Обновление :").Write(10, 10, Color::WHITE);
}
