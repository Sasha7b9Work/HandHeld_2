#include "defines.h"
#include "Modules/PAN3060/chirp_rf.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/HAL/HAL.h"
#include <gd32e23x.h>


static volatile uint8 rf_reply = 0;


static void delay_us(uint us)
{
    volatile uint counter = us * 10;

    while (counter-- != 0)
    {
    }
}

static uint8 spi_readwrite(uint8 byte)
{
    while (RESET == spi_i2s_flag_get(SPI_PAN3060, SPI_FLAG_TBE))
    {
        int i = 0;
    }

    spi_i2s_data_transmit(SPI_PAN3060, byte);

//    while (RESET == spi_i2s_flag_get(SPI_PAN3060, SPI_FLAG_RBNE))
//    {
//    }
    
    while (RESET == spi_i2s_flag_get(SPI_PAN3060, SPI_FLAG_TBE))
    {
        int i = 0;
    }

    uint8 result = (uint8)spi_i2s_data_receive(SPI_PAN3060);
    
    return result;
}


#define spi_cs_set_low()            pinSPI1_NSS.ToLow()
#define spi_cs_set_high()           pinSPI1_NSS.ToHi()


/**
 * @brief change rf mode from deep sleep to standby3(STB3)
 * @param[in] <none>
 * @return result
 */
void rf_init(void)
{
    uint8 _data;
    uint8 _cal30, _cal31, _cal32;

#ifdef SPI_SOFT_3LINE
    rf_write_reg(REG_SYS_CTL, 0x03);
    rf_write_reg(0x1A, 0x83);
#endif

    //deepsleep wakeup
    _data = rf_read_reg(0x04);
    _data |= 0x10;
    rf_write_reg(0x04, _data);
    delay_us(10);
    _data &= 0xEF;
    rf_write_reg(0x04, _data);
    if (rf_reply != RF_OK)
        return;
    _data = rf_read_reg(REG_SYS_CTL);
    _data &= 0x7F;
    rf_write_reg(REG_SYS_CTL, _data);
    delay_us(10);
    _data |= 0x80;
    rf_write_reg(REG_SYS_CTL, _data);
    delay_us(10);
    _data &= 0x7F;
    rf_write_reg(REG_SYS_CTL, _data); //-V525
    delay_us(10);

    rf_write_reg(REG_OP_MODE, RF_MODE_DEEP_SLEEP);
    delay_us(10);

    rf_write_reg(REG_OP_MODE, RF_MODE_SLEEP);
    delay_us(10);

    //set bit5 in register 0x06 at page 3
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x06);
    _data |= 1 << 5;
    rf_write_spec_page_reg(PAGE3_SEL, 0x06, _data);
    delay_us(10);

    rf_write_reg(REG_OP_MODE, RF_MODE_STB1);
    delay_us(10);

    rf_write_spec_page_reg(PAGE3_SEL, 0x26, 0x2F);
    delay_us(10);

    rf_write_reg(0x04, 0x36); //-V525
    delay_us(10);

    //rf_tcxo_init();

    rf_write_reg(REG_OP_MODE, RF_MODE_STB2);
    delay_us(2000);

    rf_write_reg(REG_OP_MODE, RF_MODE_STB3);
    delay_us(10);

    //rf_ft_calibr
    rf_efuse_on();

    _cal30 = rf_efuse_read_encry_byte(30);
    _cal31 = rf_efuse_read_encry_byte(31);
    _cal32 = rf_efuse_read_encry_byte(32);

    if (rf_efuse_read_encry_byte(28) == 0x5A)
    {
        rf_write_spec_page_reg(PAGE2_SEL, 0x3D, 0xFD);

        if (_cal32 != 0)
            rf_write_spec_page_reg(PAGE0_SEL, 0x45, _cal32);

        if (rf_efuse_read_encry_byte(13) == MODEM_MPA)
        {
            _data = rf_read_spec_page_reg(PAGE3_SEL, 0x1C);
            _data &= 0xE0;
            _data |= _cal30 & 0x1F;
            rf_write_spec_page_reg(PAGE3_SEL, 0x1C, _data);
        }
        else
            if (rf_efuse_read_encry_byte(13) == MODEM_MPB)
            {
                _data = (uint8)(0xc0 | (uint8)(_cal30 & 0x1f));
                rf_write_spec_page_reg(PAGE3_SEL, 0x1C, _data);
            }

        rf_write_spec_page_reg(PAGE3_SEL, 0x1D, _cal31);
    }

    rf_efuse_off();

    //init rf registers
    for (uint8 i = 0; i < sizeof(g_reg_cfg) / sizeof(pan_reg_cfg_t); i++)
    {
        rf_write_spec_page_reg(g_reg_cfg[i].page, g_reg_cfg[i].addr, g_reg_cfg[i].data);
    }

    //set AGC on
    _data = rf_read_spec_page_reg(PAGE2_SEL, 0x06);
    _data &= 0xFE;
    rf_write_spec_page_reg(PAGE2_SEL, 0x06, _data);

    rf_write_fifo_otp(PAGE2_SEL, 0x0A, (const uint8 *)reg_agc_freq400, 40);

    rf_write_spec_page_reg(PAGE2_SEL, 0x34, 0xef);

    //init antenna and LED outputs, 
    //  all gpio as outputs
    rf_write_spec_page_reg(PAGE0_SEL, 0x65, (1 << RF_GPIO_3) | (1 << RF_GPIO_0));
    rf_write_spec_page_reg(PAGE0_SEL, 0x66, (1 << RF_GPIO_11) | (1 << RF_GPIO_10));

    //  tx (gpio0) = 0, rx (gpio10) = 1, led (gpio11) = 0 (len on)
    rf_write_spec_page_reg(PAGE0_SEL, 0x67, (0 << RF_GPIO_3) | (0 << RF_GPIO_0));
    rf_write_spec_page_reg(PAGE0_SEL, 0x68, (0 << RF_GPIO_11) | (1 << RF_GPIO_10));

    //    rf_set_gpio_input(MODULE_GPIO_CAD_IRQ);                //CAD not used
    //    rf_set_gpio_state(MODULE_GPIO_CAD_IRQ, 0);
}

/**
 * @brief read one byte from register in current page
 * @param[in] <addr> register address to write
 * @return value read from register
 */
uint8 rf_read_reg(uint8 _addr)
{
    uint8 _data;

    spi_cs_set_low();
    spi_readwrite((uint8)(_addr << 1));
    _data = spi_readwrite(0x00);
    spi_cs_set_high();

    return _data;
}

/**
 * @brief write global register in current page and chick
 * @param[in] <addr> register address to write
 * @param[in] <value> address value to write to rgister
 * @return result
 */
void rf_write_reg(uint8 _addr, uint8 _data)
{
    spi_cs_set_low();
    spi_readwrite((uint8)((_addr << 1) | 0x01));
    spi_readwrite(_data);
    spi_cs_set_high();

    rf_reply = RF_OK;
#if SPI_WRITE_CHECK
    if (rf_read_reg(_addr) != _data)
        rf_reply = RF_FAIL;
#endif
}

void rf_write_fifo_otp(enum PAGE_SEL /*_page*/, uint8 _addr, const uint8 *_buffer, uint8 _cnt)
{
    spi_cs_set_low();
    spi_readwrite((uint8)((_addr << 1) | 0x01));

    do
    {
        spi_readwrite(*_buffer++);
    } while (--_cnt);
    spi_cs_set_high();
}

/**
 * @brief rf receive data fifo,read bytes from register
 * @param[in] <_addr> register address to write
 * @param[in] <_buffer> receive data buffer
 * @param[in] <_cnt> receive data size
 * @return none
 */
void rf_read_fifo(uint8 _addr, uint8 *_buffer, uint8 _cnt)
{
    spi_cs_set_low();
    spi_readwrite((uint8)((_addr << 1) | 0x00));
    do
    {
        *_buffer++ = spi_readwrite(0x00);
    } while (--_cnt);
    spi_cs_set_high();
}

/**
 * @brief switch page
 * @param[in] <page> page to switch
 * @return result
 */
void rf_switch_page(enum PAGE_SEL _page)
{
    rf_write_reg(REG_SYS_CTL, (uint8)(_page & 0x03));
}

/**
 * @brief read a value to register in specific page
 * @param[in] <page> the page of register
 * @param[in] <addr> register address
 * @return register value
 */
uint8 rf_read_spec_page_reg(enum PAGE_SEL _page, uint8 _addr)
{
    rf_switch_page(_page);

    return rf_read_reg(_addr);
}

/**
 * @brief This function write a value to register in specific page
 * @param[in] <page> the page of register
 * @param[in] <addr> register address
 * @param[in] <value> value to write
 * @return result
 */
void rf_write_spec_page_reg(enum PAGE_SEL _page, uint8 _addr, uint8 _data)
{
    rf_switch_page(_page);
    if (rf_reply != RF_OK)
        return;

    rf_write_reg(_addr, _data);
}

/**
 * @brief efuse function enable
 * @param[in] <none>
 * @return  result
 */
void rf_efuse_on(void)
{
    uint8 _data;

    _data = rf_read_spec_page_reg(PAGE2_SEL, 0x3E);
    _data &= ~(1 << 3);
    rf_write_spec_page_reg(PAGE2_SEL, 0x3E, _data);
}

/**
 * @brief efuse function disable
 * @param[in] <none>
 * @return  result
 */
void rf_efuse_off(void)
{
    uint8 _data;

    _data = rf_read_spec_page_reg(PAGE2_SEL, 0x3E);
    _data |= 1 << 3;
    rf_write_spec_page_reg(PAGE2_SEL, 0x3E, _data);
}

/**
 * @brief read efuse data for initialize
 * @return data
 */
uint8 rf_efuse_read_encry_byte(uint8 _efuse_addr)
{
    uint8 _tmp, _timeout = 100;

    rf_switch_page(PAGE2_SEL);

    spi_cs_set_low();
    spi_readwrite((0x3B << 1) | 0x01);
    spi_readwrite(0x5A);
    spi_readwrite(0xA5);
    spi_readwrite((uint8)(_efuse_addr << 1));
    spi_cs_set_high();

    do
    {
        _tmp = rf_read_spec_page_reg(PAGE0_SEL, 0x6C);
        if (_tmp & 0x80)
        {
            break;
        }
    } while (_timeout--);

    return rf_read_spec_page_reg(PAGE2_SEL, 0x3B);
}

/**
 * @brief change rf mode from standby3(STB3) to deep sleep, rf should set DCDC_OFF before enter deepsleep
 * @param[in] <none>
 * @return result
 */
void rf_deepsleep(void)
{
    uint8 _data;

    //    rf_port.antenna_close();
    //    rf_port.delayus(10);

    rf_write_reg(REG_OP_MODE, RF_MODE_STB3); //-V525
    delay_us(150);
    rf_write_reg(REG_OP_MODE, RF_MODE_STB2);
    delay_us(10);
    rf_write_reg(REG_OP_MODE, RF_MODE_STB1);
    delay_us(10);
    rf_write_reg(0x04, 0x06);
    delay_us(10);
    rf_write_reg(REG_OP_MODE, RF_MODE_SLEEP);
    delay_us(10);

    //reset bit5 in register 0x06 at page 3
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x06);
    _data &= ~(1 << 5);
    rf_write_spec_page_reg(PAGE3_SEL, 0x06, _data);
    delay_us(10);

    rf_write_spec_page_reg(PAGE3_SEL, 0x26, 0x0F);
    delay_us(10);

    rf_write_reg(REG_OP_MODE, RF_MODE_DEEP_SLEEP);
}

/**
 * @brief set rf default para
 * @param[in] <none>
 * @return result
 */
void rf_set_default_para(void)
{
    uint8 _data;

    //set frequency
    rf_write_spec_page_reg(PAGE0_SEL, 0x40, 0x3A);

    _data = rf_read_spec_page_reg(PAGE0_SEL, 0x3d);
    _data &= ~(0x70);
    _data |= 0x10;
    rf_write_spec_page_reg(PAGE0_SEL, 0x3d, _data); //-V525

    rf_write_spec_page_reg(PAGE3_SEL, 0x15, 0x22);
    rf_write_spec_page_reg(PAGE3_SEL, 0x16, 0xC0);
    rf_write_spec_page_reg(PAGE3_SEL, 0x17, 0x00);

    //433920000 = 0x19DD1800
    rf_write_spec_page_reg(PAGE3_SEL, 0x09, 0x00);
    rf_write_spec_page_reg(PAGE3_SEL, 0x0A, 0x18);
    rf_write_spec_page_reg(PAGE3_SEL, 0x0B, 0xDD);
    rf_write_spec_page_reg(PAGE3_SEL, 0x0C, 0x19);

    //!!rf_set_refresh                //hz nado ili net

    //set bandwidth 125kHz and code rate 4/5
    rf_write_spec_page_reg(PAGE3_SEL, 0x0D, 0x72);

    //set spread factor 6 (64 chips/symb) and CRC on
    rf_write_spec_page_reg(PAGE3_SEL, 0x0E, 0x68);

    //clear bit1 at 0x3F register for 500k bandwidth
    _data = rf_read_spec_page_reg(PAGE2_SEL, 0x3F);
    _data |= 0x02;
    rf_write_spec_page_reg(PAGE2_SEL, 0x3F, _data);

    //set power 14 (ramp 0x0B, trim 0x02, ldo 0x01)
    //  power ramp
    _data = rf_read_spec_page_reg(PAGE0_SEL, 0x1E);
    _data &= 0xC0;
    _data |= 0x0B;
    rf_write_spec_page_reg(PAGE0_SEL, 0x1E, _data);

    //  power trim
    _data = rf_read_spec_page_reg(PAGE0_SEL, 0x4B);
    _data &= 0xF0;
    _data |= 0x02;
    rf_write_spec_page_reg(PAGE0_SEL, 0x4B, _data);

    //  power ldo
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x22);
    _data |= 0x01;
    rf_write_spec_page_reg(PAGE3_SEL, 0x22, _data);

    //  power pa duty
    _data = rf_read_spec_page_reg(PAGE0_SEL, 0x46);
    _data |= 0x04;
    rf_write_spec_page_reg(PAGE0_SEL, 0x46, _data);

    //  power pa bias
    rf_efuse_on();
    _data = rf_efuse_read_encry_byte(32);
    rf_efuse_off();
    if (_data == 0)
        _data = 8;
    _data |= 0x80;
    rf_write_spec_page_reg(PAGE0_SEL, 0x45, _data);

    //set ldo on (dcdc off) regulator
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x24);
    _data &= ~(1 << 3);
    rf_write_spec_page_reg(PAGE3_SEL, 0x24, _data);
}

/**
 * @brief change rf mode from sleep to standby3(STB3)
 * @param[in] <none>
 * @return result
 */
void rf_sleep_wakeup(void)
{
    /*
    RF_ASSERT(rf_set_spec_page_reg_bits(PAGE3_SEL, 0x06, BIT5));
    rf_port.delayus(10);

    RF_ASSERT(rf_write_reg(REG_OP_MODE, RF_MODE_STB1));
    rf_port.delayus(10);

    RF_ASSERT(rf_write_spec_page_reg(PAGE3_SEL, 0x26, 0x2f));
    rf_port.delayus(10);

    RF_ASSERT(rf_write_reg(0x04, 0x36));
    rf_port.delayus(10);

    rf_port.tcxo_init();

    RF_ASSERT(rf_write_reg(REG_OP_MODE, RF_MODE_STB2));
    rf_port.delayus(150);

    RF_ASSERT(rf_write_reg(REG_OP_MODE, RF_MODE_STB3));

    rf_port.delayus(10);
    rf_port.antenna_init();

    return OK;
    */
}

/**
 * @brief rf enter single tx mode and send packet
 * @param[in] <_buffer> buffer contain data to send
 * @param[in] <_cnt> the length of data to send
 * @return result
 */
void rf_single_tx_data_otp(const uint8 *_buffer, uint8 _cnt)
{
    uint8 _data;

    //set mode STB3
    rf_write_reg(REG_OP_MODE, RF_MODE_STB3);

    //set ldo pa on
    _data = rf_read_spec_page_reg(PAGE0_SEL, 0x4F);
    _data |= 1 << 3;
    rf_write_spec_page_reg(PAGE0_SEL, 0x4F, _data);

    //set single tx mode
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x06);
    _data &= ~(1 << 2);
    rf_write_spec_page_reg(PAGE3_SEL, 0x06, _data);

    //set payload length
    rf_write_spec_page_reg(PAGE1_SEL, REG_PAYLOAD_LEN, PACKET_PAYLOAD_LENGTH);

    //set mode TX
    rf_write_reg(REG_OP_MODE, RF_MODE_TX);

    //write fifo buffer
    rf_write_fifo_otp(PAGE1_SEL, REG_FIFO_ACC_ADDR, _buffer, _cnt);
}

/**
 * @brief rf enter rx continous mode to receive packet
 * @param[in] <none>
 * @return result
 */
void rf_enter_continous_rx(void)
{
    uint8 _data;

    rf_write_reg(REG_OP_MODE, RF_MODE_STB3);    //set mode

    //set continuos receive mode
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x06);
    _data &= 0xFC;
    _data |= RF_RX_CONTINOUS;
    rf_write_spec_page_reg(PAGE3_SEL, 0x06, _data);

    rf_write_reg(REG_OP_MODE, RF_MODE_RX);      //set mode
}

/**
 * @brief rf enter rx single timeout mode to receive packet
 * @param[in] <timeout> rx single timeout time(in ms)
 * @return result
 */
void rf_enter_single_timeout_rx(uint16 _timeout)
{
    uint8 _data;

    rf_write_reg(REG_OP_MODE, RF_MODE_STB3);    //set mode

    //set single receive mode
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x06);
    _data &= 0xFC;
    _data |= RF_RX_SINGLE_TIMEOUT;
    rf_write_spec_page_reg(PAGE3_SEL, 0x06, _data);

    //set timeout value
    rf_write_spec_page_reg(PAGE3_SEL, 0x07, (uint8)_timeout);
    rf_write_spec_page_reg(PAGE3_SEL, 0x08, (uint8)(_timeout >> 8));

    rf_write_reg(REG_OP_MODE, RF_MODE_RX);      //set mode
}

/**
 * @brief change rf mode from standby3(STB3) to sleep, rf should set DCDC_OFF before enter sleep
 * @param[in] <none>
 * @return result
 */
void rf_sleep(void)
{
    uint8 _data;

    rf_write_reg(REG_OP_MODE, RF_MODE_STB3); //-V525
    if (rf_reply == RF_FAIL)
        return;
    delay_us(150);                          //150us

    rf_write_reg(REG_OP_MODE, RF_MODE_STB2);
    if (rf_reply == RF_FAIL)
        return;
    delay_us(10);                    //10us

    rf_write_reg(REG_OP_MODE, RF_MODE_STB1);
    if (rf_reply == RF_FAIL)
        return;
    delay_us(10);                    //10us

    rf_write_reg(0x04, 0x16);
    if (rf_reply == RF_FAIL)
        return;
    delay_us(10);                    //10us

    rf_write_reg(REG_OP_MODE, RF_MODE_SLEEP);
    if (rf_reply == RF_FAIL)
        return;

    //reset bit 5 in register 0x06 at page 3
    _data = rf_read_spec_page_reg(PAGE3_SEL, 0x06);
    _data &= ~(1 << 5);
    rf_write_spec_page_reg(PAGE3_SEL, 0x06, _data);
    if (rf_reply == RF_FAIL)
        return;
    delay_us(10);                    //10us

    rf_write_spec_page_reg(PAGE3_SEL, 0x26, 0x0F);
}

/**
 * @brief RF clear all irq
 * @param[in] <none>
 * @return result
 */
uint8 rf_clr_irq(void)
{
    uint8 clr_cnt = 0, reg_value;
    uint16 a = 0, b = 0;
    while (clr_cnt < 3)
    {
        rf_write_spec_page_reg(PAGE0_SEL, 0x6C, 0x3f);  //clr irq
        reg_value = rf_read_spec_page_reg(PAGE0_SEL, 0x6C);
        if ((reg_value & 0x7f) == 0) //-V547
        {
            return OK;
        }
        else
        {
            clr_cnt++;
            for (a = 0; a < 1200; a++)
                for (b = 0; b < 100; b++);
            continue;
        }
    }
    return FAIL;
}
