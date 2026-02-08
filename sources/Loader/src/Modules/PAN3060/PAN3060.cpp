// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/PAN3060/chirp_rf.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/String.h"
#include "Hardware/Timer.h"
#ifdef GD32E230
	#include <gd32e23x.h>
#endif
#ifdef GD32F303
	#include <gd32f30x.h>
#endif

#define PAN30XX_IRQ_PORT 			GPIOA
#define PAN30XX_IRQ_PIN 			GPIO_PIN_8
#ifdef GD32E230
#define PAN30XX_IRQ_EXTI_SRC 	EXTI_SOURCE_GPIOA
#define PAN30XX_IRQ_EXTI_PIN 	EXTI_SOURCE_PIN8
#endif
#ifdef GD32F303
#define PAN30XX_IRQ_EXTI_SRC 	GPIO_PORT_SOURCE_GPIOA
#define PAN30XX_IRQ_EXTI_PIN 	GPIO_PIN_SOURCE_8
#endif
#define PAN30XX_IRQ_EXTI 			EXTI_8

extern uint rx_irq_set_f;
extern uint8 rx_buffer[134];

namespace PAN3060
{
    static bool in_process_upgrade = false;     // ќбновление в процессе
    static bool is_finished = false;            // ќбновление завершено

    static void InitIRQ();

    static void InitSPI();

    static const uint NUM_PAGES = 54;
    static const uint BEGIN_FIRMWARE = 0x8002000;
    static const uint SIZE_CHAIN = 128;
    static const int CHAINS_IN_PAGE = 8;

    static int chains_is_ok = 0;
    static int chains_is_fail = 0;

    // —труктура используетс€ дл€ приЄма пакетов
    struct Packet
    {
        uint8 buffer[200];
        uint8 length;
        uint8 number_chain_in_page; // Ќомер чайна в странице, которой он принадлежит
        uint8 number_page;          // Ётой странице принадлежит чайн. Ќумераци€ с 0

        bool IsValid() const;

        void ReceiveChain();
        void ReceiveFinish();
        uint8 CalculateNumberPage(uint16 number_chain_full) const;          // –ассчитывает номер страницы, которой принадлежит чайн
        uint8 CalculateChainInPage(uint16 number_chain_full) const;         // ѕреобразует сквозной номер чайна в номер чайна на странице
    };

    // Ёта структура описывает все данные прошивки
    struct Firmware
    {
        bool pages[NUM_PAGES];      // true означает, что страница прин€та и сохранена в EEPROM
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

    // ќписывает одну страницу
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

        // «аписывает page[1024] в EEPROM
        void WritePageEEPROM() const;
    };

    // ѕринимаема€ прошивка
    static Firmware firmware;

    // ѕринимаема€ страница
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
}


void PAN3060::InitIRQ()
{
    // »нициализируем пин клоков от приЄмника на прерывание
		#ifdef GD32E230
    gpio_mode_set(PAN30XX_IRQ_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PAN30XX_IRQ_PIN);
    nvic_irq_enable(EXTI4_15_IRQn, 2);
    syscfg_exti_line_config(PAN30XX_IRQ_EXTI_SRC, PAN30XX_IRQ_EXTI_PIN);
		#endif
		#ifdef GD32F303
		gpio_init(PAN30XX_IRQ_PORT, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, PAN30XX_IRQ_PIN);
    nvic_irq_enable(EXTI5_9_IRQn, 0, 0);
		gpio_exti_source_select(PAN30XX_IRQ_EXTI_SRC, PAN30XX_IRQ_EXTI_PIN);
		#endif
		exti_init(PAN30XX_IRQ_EXTI, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(PAN30XX_IRQ_EXTI);
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
    uint irq = rf_read_spec_page_reg(PAGE0_SEL, 0x6C);

    if(irq & REG_IRQ_RX_DONE)
    {
				uint _length;

        _length = rf_read_spec_page_reg(PAGE1_SEL, 0x7D);

        rf_clr_irq();

				//received 128 bytes of firmware
        if (_length == 2 + 128 + 4)
        {
						rx_irq_set_f = 1;
						rf_read_fifo(REG_FIFO_ACC_ADDR, rx_buffer, _length);
        }
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



bool PAN3060::InProcessUpgrade()
{
    if (is_finished)
    {
        return false;
    }

    return in_process_upgrade;
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
