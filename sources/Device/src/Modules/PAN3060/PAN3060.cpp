// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/PAN3060/chirp_rf.h"
#include "system.h"


namespace PAN3060
{
    /*
    *   IRQ  - PA8
    *   NSS  - PB12     SPI1_NSS
    *   SCK  - PB13     SPI1_SCK    AF_0
    *   MISO - PB14     SIP1_MISO   AF_0
    *   MOSI - PB15     SPI1_MOSI   AF_0
    */

    static bool need_rx = false;

    static void InitIRQ();

    static void InitSPI();
}


void PAN3060::InitFull()
{
    pinSPI1_NSS.Init();
    pinSPI1_NSS.ToHi();

    InitIRQ();

    InitSPI();

    InitRF();
}


void PAN3060::InitRF()
{
    rf_init();

    rf_set_default_para();

    rf_enter_continous_rx();
}


void PAN3060::InitIRQ()
{
#ifdef MODEL7735

    // Инициализируем пин клоков от приёмника на прерывание
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    nvic_irq_enable(EXTI4_15_IRQn, 2);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN8);
    exti_init(EXTI_8, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_8);

#endif
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
    if (need_rx)
    {
        need_rx = false;

        uint8_t _irq;

        _irq = rf_read_spec_page_reg(PAGE0_SEL, 0x6C);
        if (_irq & REG_IRQ_RX_TIMEOUT)
        {
            rf_clr_irq();
        }

        if (_irq & REG_IRQ_RX_DONE)
        {
            uint8_t _buffer[PACKET_PAYLOAD_LENGTH];
            uint8_t _len = rf_read_spec_page_reg(PAGE1_SEL, 0x7D);

            rf_read_fifo(REG_FIFO_ACC_ADDR, _buffer, PACKET_PAYLOAD_LENGTH);
            rf_clr_irq();
            if (_len == PACKET_PAYLOAD_LENGTH && _buffer[0] == VIBROLINE_HEAD)
            {
                _buffer[1] &= 0x7F;
                if (_buffer[1] & VIBROLINE_DEVICE_DOORBELL)
                {
                    Source::Receive(Source::DoorBell);
                }
                else if (_buffer[1] & VIBROLINE_DEVICE_PHONE)
                {
                    Source::Receive(Source::PhoneHome);
                }
                else if (_buffer[1] & VIBROLINE_DEVICE_INTERCOM)
                {
                    Source::Receive(Source::Intercom);
                }
                else if (_buffer[1] & VIBROLINE_DEVICE_BABYCRY)
                {
                    Source::Receive(Source::Microphone);
                }
                else if (_buffer[1] & VIBROLINE_DEVICE_MOBILE)
                {
                    Source::Receive(Source::Mobile);
                }

                rf_init();
                rf_set_default_para();
                rf_enter_continous_rx();
            }
        }
    }
}


void PAN3060::PrepareToSleep()
{
    rf_deepsleep();
}


void PAN3060::CallbackOnIRQ()
{
    need_rx = true;
}


void PAN3060::CallbackOnWakeUp()
{
}
