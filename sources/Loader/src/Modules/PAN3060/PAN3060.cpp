// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/PAN3060/chirp_rf.h"
#include <gd32e23x.h>
#include <cstring>


namespace PAN3060
{
    /*
    *   IRQ  - PA8
    *   NSS  - PB12     SPI1_NSS
    *   SCK  - PB13     SPI1_SCK    AF_0
    *   MISO - PB14     SIP1_MISO   AF_0
    *   MOSI - PB15     SPI1_MOSI   AF_0
    */

    static uint time_enable = 0;        // Время, когда начались клоки

    static bool need_rx = false;

    static void InitIRQ();

    static void InitSPI();
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

//    //                                                                   SCK           MISO          MOSI
//    gpio_mode_set          (GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,     GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
//    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
//    gpio_af_set            (GPIOB, GPIO_AF_0,                        GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
//
//    spi_parameter_struct spi_init_struct;
//
//    spi_i2s_deinit(SPI_PAN3060);
//    spi_struct_para_init(&spi_init_struct);
//
//    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
//    spi_init_struct.device_mode = SPI_MASTER;
//    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
//    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
//    spi_init_struct.nss = SPI_NSS_SOFT;
//    spi_init_struct.prescale = SPI_PSC_8;
//    spi_init_struct.endian = SPI_ENDIAN_MSB;
//    spi_init(SPI_PAN3060, &spi_init_struct);
//
//    spi_enable(SPI_PAN3060);
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

                }
                if (_buffer[1] & VIBROLINE_DEVICE_PHONE)
                {

                }

                if (_buffer[1] & VIBROLINE_DEVICE_INTERCOM)
                {

                }
                if (_buffer[1] & VIBROLINE_DEVICE_BABYCRY)
                {

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
#ifdef WIN32
#else
    EXTI_PD = EXTI_8;
    EXTI_INTEN |= EXTI_8;
#endif
}


void PAN3060::CallbackOnIRQ()
{
    need_rx = true;
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
