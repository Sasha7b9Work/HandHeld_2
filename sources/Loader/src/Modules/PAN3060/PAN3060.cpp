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
    static const uint SIZE_FIRMWARE = NUM_PAGES * 1024;
    static const uint SIZE_CHAIN = 128;
    static const int CHAINS_IN_PAGE = 8;

    // Здесь хранятся контрольные суммы для каждого пакета
    static const uint ALL_CHAINS = SIZE_FIRMWARE / SIZE_CHAIN;
    static uint crc[ALL_CHAINS];                                    // Здесь будут храниться контрольные суммы для всех чайнов
    static uint crc_page[CHAINS_IN_PAGE];                           // А здесь для чайнов текущей принимаемой страницы

    static uint8 page[1024];                                        // Здесь будет принятая страница, котору мы целиком запишем в память

    static int chains_is_ok = 0;
    static int chains_is_fail = 0;
    static uint main_crc = 0;
    static int prev_page = -1;                                      // На этой странице находился предыдущий принятый чайн

    static void Reset();

    // Эту функцию вызываем, когда контрольная сумма в eeprom не совпала
    static void FullReset();

    // Cтолько чайнов принято
    static int ReceivedChains();

    // Все ли чайны приняты
    static bool AllChaninsReceived();

    // Проверить на завершение - всё принято и всё соотвествует
    static void CheckForCompletion();

    // Структура используется для приёма пакетов
    struct Packet
    {
        uint8 buffer[200];
        uint8 length;
        uint16 number_chain_full;   // Сквозной номер чайна
        uint8 number_chain_in_page; // Номер чайна в странице, которой он принадлежит
        uint8 number_page;          // Этой странице принадлежит чайн. Нумерация с 0

        bool IsValid() const;

        void ReceiveChain();
        void ReceiveFinish();
        uint8 CalcualteNumberPage() const;          // Рассчитывает номер страницы, которой принадлежит чайн
        uint8 CalculateChainInPage() const;         // Преобразует сквозной номер чайна в номер чайна на странице


        // Стирает страницу в EEPROM, содержащую данный пакет
        void ErasePageEEPROM() const;

        // Записывает page[1024] в EEPROM
        void WritePageEEPROM() const;
    };

    // Эта структура описывает все данные прошивки
    struct Firmware
    {
        bool pages[NUM_PAGES];      // true означает, что страница принята и сохранена в EEPROM
        uint crc;
    };

    // Описывает одну страницу
    struct Page
    {
        struct Chain
        {
            uint8 buffer[SIZE_CHAIN];
            uint  crc = 0;
        };

        Chain chains[CHAINS_IN_PAGE];

        int num_page = -1;
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
    prev_page = -1;

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
    number_chain_full = Struct16(buffer).u16;

    number_page = CalcualteNumberPage();

    if (prev_page != number_page)
    {
        Reset();
    }

    number_chain_in_page = CalculateChainInPage();

    if (crc_page[number_chain_in_page] == 0)
    {
        crc_page[number_chain_in_page] = Struct32(buffer + 2 + 128).u32;

        std::memcpy(page + (uint)number_chain_in_page * SIZE_CHAIN, buffer + 2, SIZE_CHAIN);
    }

    // Если чайн последний в странице: 7, 15, 23 и т.д., то нужно сохранить страницу в ПЗУ.
    // Передатчик делает для этого паузу
    if (((number_chain_full + 1) % CHAINS_IN_PAGE) == 0)
    {
        uint *crc_full = crc + number_page * CHAINS_IN_PAGE;
        uint *crc_part = crc_page;

        {                                                   // Стираем страницу, если запись в неё ещё ни разу не производилась
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
                ErasePageEEPROM();
            }
        }

        {                                                   // Сохраняем страницу в ПЗУ, если нужно
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
                WritePageEEPROM();
            }
        }

        CheckForCompletion();
    }

    prev_page = number_page;
}


void PAN3060::Packet::ReceiveFinish()
{
    main_crc = Struct32(buffer + 2).u32;

    CheckForCompletion();
}


void PAN3060::CheckForCompletion()
{
    if (main_crc && AllChaninsReceived())
    {
        uint crc_firmware = SU::CalculateCRC32((const void *)BEGIN_FIRMWARE, SIZE_FIRMWARE);

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


uint8 PAN3060::Packet::CalcualteNumberPage() const
{
    return (uint8)(number_chain_full / 8);
}


void PAN3060::Packet::ErasePageEEPROM() const
{
    HAL_ROM::ErasePage(BEGIN_FIRMWARE + (uint)(number_page * 1024));
}


void PAN3060::Packet::WritePageEEPROM() const
{
    HAL_ROM::WritePage(BEGIN_FIRMWARE + (uint)(number_page * 1024), page);
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


uint8 PAN3060::Packet::CalculateChainInPage() const
{
    return (uint8)(number_chain_full % CHAINS_IN_PAGE);
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
    int x3 = 100;
    int y = 5;
    int dy = 10;

    Text<>("Upgrade").Write(x1, y);

    y += dy;

    Text<>("Received").Write(x1, y);

    char buffer[32];

    Text<>(SU::IntToASCII(ReceivedChains(), buffer)).Write(x2, y);

    Text<>(SU::IntToASCII((int)ALL_CHAINS, buffer)).Write(x3, y);

    y += dy;

    Text<>("Good").Write(x1, y);

    Text<>(SU::IntToASCII(chains_is_ok, buffer)).Write(x2, y);

    y += dy;

    Text<>("Bad").Write(x1, y);

    Text<>(SU::IntToASCII(chains_is_fail, buffer)).Write(x2, y);
}
