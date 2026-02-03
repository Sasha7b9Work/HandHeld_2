// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
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
    static uint8 irq = 0;           // Сюда читаем значение регистра прямо в прерывании, чтобы потом его обрабатывать

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

#ifdef MODEL7735
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN8);
    exti_interrupt_flag_clear(EXTI_8);
#endif
}


void PAN3060::InitIRQ()
{
#ifdef MODEL7735

    // Инициализируем пин клоков от приёмника на прерывание
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    nvic_irq_enable(EXTI4_15_IRQn, 2);
    exti_init(EXTI_8, EXTI_INTERRUPT, EXTI_TRIG_RISING);

#endif
}


void PAN3060::InitSPI()
{
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    spi_parameter_struct spi_is;
    spi_i2s_deinit(SPI_PAN3060);
    spi_struct_para_init(&spi_is);

    spi_is.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_is.device_mode = SPI_MASTER;
    spi_is.frame_size = SPI_FRAMESIZE_8BIT;
    spi_is.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_is.nss = SPI_NSS_SOFT;
    spi_is.prescale = SPI_PSC_8;
    spi_is.endian = SPI_ENDIAN_MSB;
    spi_init(SPI_PAN3060, &spi_is);

    spi_fifo_access_size_config(SPI1, SPI_BYTE_ACCESS);
    spi_enable(SPI_PAN3060);
}


void PAN3060::Update()
{
    if (need_rx && irq != 0x00)
    {
        need_rx = false;

        if (irq & REG_IRQ_RX_TIMEOUT)
        {
            rf_clr_irq();
        }

        if (irq & REG_IRQ_RX_DONE)
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

        irq = 0x00;
    }
}


void PAN3060::PrepareToSleep()
{
#ifdef MODEL7735
    syscfg_exti_line_clear(EXTI_SOURCE_PIN8);
#endif

    rf_deepsleep();
}


void PAN3060::CallbackOnIRQ()
{
    irq = rf_read_spec_page_reg(PAGE0_SEL, 0x6C);

    if(irq != 0x00)                 // После захода в спящий режим вызывается это прерывание
    {                               // Но чтение регистра прерываний даёт 0. Проверяем, что это прерывание вызвано именно приёмом даных
        need_rx = true;
    }
}


void PAN3060::CallbackOnWakeUp()
{
    InitRF();
}
