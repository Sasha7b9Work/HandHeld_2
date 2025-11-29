// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/PAN3060/chirp_rf.h"
#include "Display/Text.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/String.h"
#include <gd32e23x.h>


namespace PAN3060
{
    static bool need_recv = false;

    static bool in_process_upgrade = false;

    static void InitIRQ();

    static void InitSPI();

    static const uint NUM_PAGES = 54;
    static const uint BEGIN_FIRMWARE = 0x8002000;
    static const uint SIZE_CHAIN = 128;
    static const int CHAINS_IN_PAGE = 8;

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

    // Эта структура описывает все данные прошивки
    struct Firmware
    {
        bool pages[NUM_PAGES];      // true означает, что страница принята и сохранена в EEPROM
        uint crc = 0;

        void CheckForComplete();

        bool IsFilled() const;

        void Clear();

        int FilledPages() const
        {
            int result = 0;

            for (uint i = 0; i < NUM_PAGES; i++)
            {
                if (pages[i])
                {
                    result++;
                }
            }

            return result;
        }

    };

    // Описывает одну страницу
    struct Page
    {
        struct Chain
        {
            uint8 buffer[SIZE_CHAIN];
        };

        Chain chains[CHAINS_IN_PAGE];

        bool received[CHAINS_IN_PAGE];

        int number = -1;

        void Clear();

        // true, если страница полностью заполнена
        bool IsFilled() const;

        // Стирает страницу в EEPROM, содержащую данный пакет
        void ErasePageEEPROM() const;

        // Записывает page[1024] в EEPROM
        void WritePageEEPROM() const;
    };

    // Принимаемая прошивка
    static Firmware firmware;

    // Принимаемая страница
    static Page page;
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

    page.Clear();

    firmware.Clear();
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
            Packet packet;

            packet.length = rf_read_spec_page_reg(PAGE1_SEL, 0x7D);

            rf_read_fifo(REG_FIFO_ACC_ADDR, packet.buffer, packet.length);

            rf_clr_irq();

            if (packet.length == 2 + 128 + 4)            // Принимаем 128 байт прошивки
            {
                if (packet.IsValid())
                {
                    chains_is_ok++;
                    packet.ReceiveChain();
                }
                else
                {
                    chains_is_fail++;
                }

                rf_init();
                rf_set_default_para();
                rf_enter_continous_rx();
            }
            else if (packet.length == 2 + 4 + 4)         // Принимаем завершающий пакет
            {
                if (packet.IsValid())
                {
                    chains_is_ok++;
                    packet.ReceiveFinish();
                }
                else
                {
                    chains_is_fail++;
                }

                rf_init();
                rf_set_default_para();
                rf_enter_continous_rx();
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
    static uint16 prev_number_chain_full = 0;
    uint16 number_chain_full = Struct16(buffer).u16;
    
    if(number_chain_full == 271 ||
        number_chain_full == 119 ||
        number_chain_full == 7)
    {
        number_chain_full = number_chain_full;
    }

    if(number_chain_full == 270 ||
    number_chain_full == 118 ||
    number_chain_full == 6)
    {
        number_chain_full = number_chain_full;
    }

    if(number_chain_full == 269 ||
        number_chain_full == 117 ||
        number_chain_full == 5)
    {
        number_chain_full = number_chain_full;
    }
    
    if(number_chain_full == 268 ||
        number_chain_full == 116 ||
        number_chain_full == 4)
    {
        number_chain_full = number_chain_full;
    }
    
    prev_number_chain_full = number_chain_full;

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

    std::memcpy(page.chains[number_chain_in_page].buffer, buffer + 2, SIZE_CHAIN);
    page.received[number_chain_in_page] = true;

    int filled = firmware.FilledPages();

    if (number_chain_in_page == CHAINS_IN_PAGE - 2)
    {
        filled = filled;
    }

    filled = filled;

    if (number_chain_in_page == CHAINS_IN_PAGE - 1)
    {
        if (!firmware.pages[number_page])               // Если данная страница ещё не записана в EEPROM
        {
            firmware.pages[number_page] = true;

            page.ErasePageEEPROM();

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


void PAN3060::Firmware::CheckForComplete()
{
    if (!IsFilled())
    {
        return;
    }

    uint crc_real = SU::CalculateCRC32((const void *)BEGIN_FIRMWARE, NUM_PAGES * 1024);

    if (crc_real == crc)
    {
        in_process_upgrade = false;
    }
    else
    {
        Clear();
        page.Clear();
    }
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
    crc = 0;

    for (uint i = 0; i < NUM_PAGES; i++)
    {
        pages[i] = false;
    }
}


uint8 PAN3060::Packet::CalculateNumberPage(uint16 number_chain_full) const
{
    return (uint8)(number_chain_full / CHAINS_IN_PAGE);
}


void PAN3060::Page::ErasePageEEPROM() const
{
    HAL_ROM::ErasePage(BEGIN_FIRMWARE + (uint)(number * 1024));
}


void PAN3060::Page::WritePageEEPROM() const
{
    HAL_ROM::WritePage(BEGIN_FIRMWARE + (uint)(number * 1024), &chains[0].buffer[0]);
}


uint8 PAN3060::Packet::CalculateChainInPage(uint16 number_chain_full) const
{
    return (uint8)(number_chain_full % CHAINS_IN_PAGE);
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
    int x1 = 5;
    int x2 = 50;
//    int x3 = 100;
    int y = 5;
    int dy = 10;

    Text<>("Upgrade").Write(x1, y);

//    y += dy;
//
//    Text<>("Filled").Write(x1, y);
//
//    char buffer[32];
//
//    Text<>(SU::IntToASCII(firmware.FilledPages(), buffer)).Write(x2, y);
//
//    y += dy;
//
//    Text<>("Good").Write(x1, y);
//
//    Text<>(SU::IntToASCII(chains_is_ok, buffer)).Write(x2, y);
//
//    y += dy;
//
//    Text<>("Bad").Write(x1, y);
//
//    Text<>(SU::IntToASCII(chains_is_fail, buffer)).Write(x2, y);
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
