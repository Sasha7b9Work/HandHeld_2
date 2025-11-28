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

    static const uint begin_firmware = 0x8002000;
    static const uint size_firmware = 54 * 1024;
    static const uint SIZE_CHAIN = 128;
    static const int CHAINS_IN_PAGE = 8;

    // Здесь хранятся контрольные суммы для каждого пакета
    static const uint ALL_CHAINS = size_firmware / SIZE_CHAIN;
    static uint crc[ALL_CHAINS];                                    // Здесь будут храниться контрольные суммы для всех чайнов
    static uint crc_page[CHAINS_IN_PAGE];                           // А здесь для чайнов текущей принимаемой страницы

    static uint8 page[1024];                                        // Здесь будет принятая страница, котору мы целиком запишем в память

    static int chains_is_ok = 0;
    static int chains_is_fail = 0;
    static uint main_crc = 0;
//    static int prev_page = -1;                                      // На этой странице находился предыдущий принятый чайн

    static void Reset();
    // Эту функцию вызываем, когда контрольная сумма в eeprom не совпала
    static void FullReset();

    static void ReceiveChainPacket(uint8 buffer[256]);
    static void ReceiveFinishPacket(uint8 buffer[256]);

    // Cтолько чайнов принято
    static int ReceivedChains();

    // Преобразует сквозной номер чайна в номер чайна на странице
    static int NumberChainInPage(uint full_number_chain);

    static int NumberPage(int full_number_chain);

    static void ErasePage(int num_page);

    static void WritePage(int num_page, uint8 buffer[1024]);

    // Все ли чайны приняты
    static bool AllChaninsReceived();

    // Проверить на завершение - всё принято и всё соотвествует
    static void CheckForCompletion();

    static bool PacketIsValid(uint8 buffer[256], int len);
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

    FullReset();
}


void PAN3060::FullReset()
{
    // Эту процедуру делаем только один раз. Если какой-то пакет будет принят с ошибкой, то в последующих циклах обновления его просто
    // добавим сюда
    for (uint i = 0; i < ALL_CHAINS; i++)
    {
        crc[i] = 0;
    }

    main_crc = 0;
    chains_is_ok = 0;
    chains_is_fail = 0;

    Reset();
}


void PAN3060::Reset()
{
    std::memset(page, 0xFF, 1024);
    std::memset(crc_page, 0x00, CHAINS_IN_PAGE * 4);
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


bool PAN3060::PacketIsValid(uint8 buffer[256], int len)
{
    Struct32 crc_recv(buffer + len - 4);

    uint crc_calc = SU::CalculateCRC32(buffer, len - 4);

    return (crc_recv.u32 == crc_calc);
}


void PAN3060::ReceiveChainPacket(uint8 buffer[256])
{
    if (!PacketIsValid(buffer, 2 + 128 + 4))
    {
        chains_is_fail++;
        return;
    }

    chains_is_ok++;

    uint16 number_chain = Struct16(buffer).u16;

    int chain_in_page = NumberChainInPage(number_chain);

    if (crc_page[chain_in_page] == 0)
    {
        crc_page[chain_in_page] = Struct32(buffer + 2 + 128).u32;

        std::memcpy(page + chain_in_page * SIZE_CHAIN, buffer + 2, SIZE_CHAIN);
    }

    // Если чайн последний в странице: 7, 15, 23 и т.д., то нужно сохранить страницу в ПЗУ
    if (((number_chain + 1) % CHAINS_IN_PAGE) == 0)
    {
        uint *crc_full = crc + NumberPage(number_chain);
        uint *crc_part = crc_page;

        bool need_erase_page = true;

        for (int i = 0; i < CHAINS_IN_PAGE; i++)
        {
            if (*(crc_full + i))                        // Если уже есть контрольная сумма, то эту страницу мы уже записывали в ПЗУ
            {
                need_erase_page = false;
                break;
            }
        }

        if (need_erase_page)
        {
            ErasePage(NumberPage(number_chain));
        }

        bool need_write_to_eeprom = false;

        for (int i = 0; i < CHAINS_IN_PAGE; i++)
        {
            if (*(crc_part + i))                        // Если принят этот чайн
            {
                if (*(crc_full + i) == 0)               // И ранее он не принят
                {
                    need_write_to_eeprom = true;

                    *(crc_full + i) = *(crc_part + i);
                }
            }
        }

        if (need_write_to_eeprom)
        {
            WritePage(NumberPage(number_chain), page);
        }

        Reset();

        CheckForCompletion();
    }
}


void PAN3060::ReceiveFinishPacket(uint8 buffer[256])
{
    if (!PacketIsValid(buffer, 2 + 4 + 4))
    {
        return;
    }

    main_crc = Struct32(buffer + 2).u32;

    CheckForCompletion();
}


void PAN3060::CheckForCompletion()
{
    if (main_crc && AllChaninsReceived())
    {
        uint crc_firmware = SU::CalculateCRC32((const void *)begin_firmware, size_firmware);

        if (crc_firmware == main_crc)
        {
            in_process_upgrade = false;
        }
        else
        {
            FullReset();
        }
    }
}


int PAN3060::NumberPage(int full_number_chain)
{
    return full_number_chain / 8;
}


void PAN3060::ErasePage(int num_page)
{
    HAL_ROM::ErasePage(begin_firmware + num_page * 1024);
}


void PAN3060::WritePage(int num_page, uint8 buffer[1024])
{
    HAL_ROM::WritePage(begin_firmware + num_page * 1024, buffer);
}


bool PAN3060::AllChaninsReceived()
{
    for (uint i = 0; i < ALL_CHAINS; i++)
    {
        if (crc[i] == 0)
        {
            return false;
        }
    }

    return true;
}


int PAN3060::NumberChainInPage(uint full_number_chain)
{
    return (int)full_number_chain % CHAINS_IN_PAGE;
}


int PAN3060::ReceivedChains()
{
    int counter = 0;

    for (uint i = 0; i < ALL_CHAINS; i++)
    {
        if (crc[i] != 0)
        {
            counter++;
        }
    }

    return counter;
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
    int y = 5;
    int dy = 10;

    Text<>("Upgrade").Write(x1, y, Color::WHITE);

    y += dy;

    Text<>("Received").Write(x1, y);

    char buffer[32];

    Text<>(SU::IntToASCII(ReceivedChains(), buffer)).Write(x2, y);

    y += dy;

    Text<>("Good").Write(x1, y);

    Text<>(SU::IntToASCII(chains_is_ok, buffer)).Write(x2, y);

    y += dy;

    Text<>("Bad").Write(x1, y);

    Text<>(SU::IntToASCII(chains_is_fail, buffer)).Write(x2, y);
}
