// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/PAN3060/chirp_rf.h"
#include "Display/Text.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/String.h"
#include "Hardware/Timer.h"
#include <gd32e23x.h>


namespace PAN3060
{
    bool in_process_upgrade = false;

    static void InitIRQ();

    static void InitSPI();

    static int chains_is_ok = 0;
    static int chains_is_fail = 0;

    // Структура используется для приёма пакетов
    struct Packet
    {
        uint8 buffer[200];
        uint8 length;
        uint8 number_chain_in_page; // Номер чайна в странице, которой он принадлежит
        uint8 number_page;          // Этой странице принадлежит чайн. Нумерация с 0

        bool IsValid() const;

        void ReceiveChain();
        void ReceiveFinish();
        uint8 CalculateNumberPage(uint16 number_chain_full) const;          // Рассчитывает номер страницы, которой принадлежит чайн
        uint8 CalculateChainInPage(uint16 number_chain_full) const;         // Преобразует сквозной номер чайна в номер чайна на странице
    };

    Firmware firmware;

    Page page;
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


void PAN3060::CallbackOnIRQ()
{
    if (!in_process_upgrade)
    {
        page.Clear();
        firmware.Clear();
    }

    in_process_upgrade = true;

    uint8 irq = rf_read_spec_page_reg(PAGE0_SEL, 0x6C);

    if (irq & REG_IRQ_RX_TIMEOUT)
    {
        rf_clr_irq();
    }

    if (irq & REG_IRQ_RX_DONE)
    {
        Packet packet;

        packet.length = rf_read_spec_page_reg(PAGE1_SEL, 0x7D);

        rf_read_fifo(REG_FIFO_ACC_ADDR, packet.buffer, packet.length);

        rf_clr_irq();

        if (packet.length == 2 + 128 + 4)            // Принимаем 128 байт прошивки
        {
            rf_init();
            rf_set_default_para();
            rf_enter_continous_rx();

            if (packet.IsValid())
            {
                chains_is_ok++;
                packet.ReceiveChain();
            }
            else
            {
                chains_is_fail++;
            }
        }
        else if (packet.length == 2 + 4 + 4)         // Принимаем завершающий пакет
        {
            rf_init();
            rf_set_default_para();
            rf_enter_continous_rx();

            if (packet.IsValid())
            {
                chains_is_ok++;
                packet.ReceiveFinish();
            }
            else
            {
                chains_is_fail++;
            }
        }
    }
}


bool PAN3060::Packet::IsValid() const
{
    Struct32 crc_recv(buffer + length - 4);

    uint crc_calc = SU::CalculateCRC32(buffer, length - 4);

    if (crc_recv.u32 == crc_calc)
    {
        return true;
    }

    return false;
}


void PAN3060::Packet::ReceiveChain()
{
    uint16 number_chain_full = Struct16(buffer).u16;
      
    number_page = CalculateNumberPage(number_chain_full);

    number_chain_in_page = CalculateChainInPage(number_chain_full);

    if (number_chain_in_page == 0)          // Первый чайн в странице, готовимся к приёму новой страницы
    {
        page.Clear();
        page.number = number_page;
    }

    if (page.number != number_page)
    {
        return;
    }

    std::memcpy(page.chains[number_chain_in_page].buffer, buffer + 2, Page::Chain::SIZE_CHAIN);
    page.received[number_chain_in_page] = true;

    if (number_chain_in_page == Page::CHAINS_IN_PAGE - 1)
    {
        if (!firmware.pages[number_page])               // Если данная страница ещё не записана в EEPROM
        {
            firmware.pages[number_page] = true;

            page.WritePageEEPROM();

            firmware.CheckForComplete();
        }
    }
}


void PAN3060::Packet::ReceiveFinish()
{
    firmware.crc = Struct32(buffer + 2).u32;

    firmware.CheckForComplete();
}


bool PAN3060::Firmware::IsFilled() const
{
    if (crc == 0)
    {
        return false;
    }

    for (uint i = 0; i < NUM_PAGES; i++)
    {
        if (!pages[i])
        {
            return false;
        }
    }

    return true;
}


void PAN3060::Firmware::Clear()
{
    for (uint i = 0; i < NUM_PAGES; i++)
    {
        HAL_ROM::ErasePage(BEGIN_FIRMWARE + (uint)(i * 1024));
    }

    crc = 0;

    for (uint i = 0; i < NUM_PAGES; i++)
    {
        pages[i] = false;
    }
}


uint8 PAN3060::Packet::CalculateNumberPage(uint16 number_chain_full) const
{
    return (uint8)(number_chain_full / Page::CHAINS_IN_PAGE);
}


void PAN3060::Page::WritePageEEPROM() const
{
    HAL_ROM::WritePage(Firmware::BEGIN_FIRMWARE + (uint)(number * 1024), &chains[0].buffer[0]);
}


uint8 PAN3060::Packet::CalculateChainInPage(uint16 number_chain_full) const
{
    return (uint8)(number_chain_full % Page::CHAINS_IN_PAGE);
}


bool PAN3060::InProcessUpgrade()
{
    return in_process_upgrade;
}


void PAN3060::FuncDraw()
{
    static TimeMeterMS meter;

    char buffer[32];

    Text<>(SU::IntToASCII(firmware.FilledPages(), buffer)).Write(1, 1);

    Text<>(SU::IntToASCII(chains_is_ok, buffer)).Write(30, 1);

    Text<>(SU::IntToASCII(chains_is_fail, buffer)).Write(60, 1);

    Text<>(SU::IntToASCII((int)(meter.ElapsedTime() / 1000), buffer)).Write(90, 1);
}


void PAN3060::Page::Clear()
{
    number = -1;

    for (int i = 0; i < CHAINS_IN_PAGE; i++)
    {
        received[i] = false;
    }
}


bool PAN3060::Page::IsFilled() const
{
    for (int i = 0; i < CHAINS_IN_PAGE; i++)
    {
        if (!received[i])
        {
            return false;
        }
    }

    return true;
}
