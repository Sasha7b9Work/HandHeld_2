#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <stdlib.h>
#include <gd32e23x.h>

/*
const uint ctrl10 = GPIO_CTL(GPIOB);
const uint pupd10 = GPIO_PUD(GPIOB);

uint ctrl = ctrl10;
ctrl &= ~GPIO_MODE_MASK(10);
ctrl &= ~GPIO_MODE_MASK(11);
ctrl |= GPIO_MODE_SET(10, GPIO_MODE_OUTPUT);
ctrl |= GPIO_MODE_SET(11, GPIO_MODE_OUTPUT);
GPIO_CTL(GPIOB) = ctrl;
uint pupd = pupd10;
pupd &= ~GPIO_PUPD_MASK(10);
pupd &= ~GPIO_PUPD_MASK(11)
pupd |= GPIO_PUPD_SET(10, GPIO_PUPD_PULLUP);
pupd |= GPIO_PUPD_SET(11, GPIO_PUPD_PULLUP);
GPIO_PUD(GPIOB) = pupd;
*/


#ifdef WIN32
    #define __asm(x)
#endif


// SCL PB10 21 - alternate I2C1
// SDA PB11 22 - alternate I2C1


#define SW_I2C_WAIT_TIME    1 // 10us 100kHz

#define I2C_SLAVE_ADDRESS7     (0xa2)   // Для часов

#define I2C_READ            0x01
#define READ_CMD            1
#define WRITE_CMD           0


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define SDA_TO_LOW()    GPIO_BC(GPIOB) = (uint)GPIO_PIN_11
#define SDA_TO_HI()     GPIO_BOP(GPIOB) = (uint)GPIO_PIN_11

#define GET_SDA()       (GPIO_ISTAT(GPIOB)&(GPIO_PIN_11))

#define SCL_TO_LOW()    GPIO_BC(GPIOB) = (uint)GPIO_PIN_10
#define SCL_TO_HI()     GPIO_BOP(GPIOB) = (uint)GPIO_PIN_10

#define DELAY()     __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP"); __asm("NOP")


static void sda_out(uint8_t out)
{
    if (out)
    {
        SDA_TO_HI();
    }
    else
    {
        SDA_TO_LOW();
    }
}


static void i2c_clk_data_out()
{
    //tLOW not less than 1,3us (but already included 100ns tSU_DAT)
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_HI();
    //tHIGH not less than 600ns
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_LOW();
}

static void i2c_start_condition()
{
    SDA_TO_HI();
    //tLOW not less than 1,3us
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_HI();
    //tSU_STA, at least 600ns
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SDA_TO_LOW();
    //tHD_STA, at least 600ns delay
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_LOW();
    //no need delay for data hold time tHD_DAT
    //DELAY();
}


static void i2c_stop_condition()
{
    SDA_TO_LOW();
    //tLOW not less than 1,3us
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_HI();
    //tSU_STO not less than 600ns
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SDA_TO_HI();
}


static uint8_t i2c_check_ack()
{
    uint8_t ack;

    //release SDA
    SDA_TO_HI();
    //tLOW not less than 1,3us (but already included 100ns tSU_DAT)
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_HI();
    //tHIGH not less than 600ns
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    ack = !GET_SDA();
    SCL_TO_LOW();
    return ack;
}

static void i2c_slave_address(uint8_t IICID, uint8_t readwrite)
{
    if (readwrite)
    {
        IICID |= I2C_READ;
    }
    else
    {
        IICID &= ~I2C_READ;
    }

    for (int x = 7; x >= 0; x--)
    {
        uint8 bit = (uint8)(IICID & (1 << x));

        sda_out(bit);
        //data setup time tSU_DAT at least 100ns
        DELAY();

        i2c_clk_data_out();
    }
}

static void i2c_register_address(uint8_t addr)
{
    for (int x = 7; x >= 0; x--)
    {
        sda_out((uint8)(addr & (1 << x)));
        //data setup time tSU_DAT at least 100ns
        DELAY();

        i2c_clk_data_out();
    }
}

static void i2c_send_ack(uint8_t ack)
{
    //set SDA, no need delay tHD_DAT=0
    if(ack == TRUE)
        SDA_TO_LOW();
    else
        SDA_TO_HI();
    //tLOW not less than 1,3us
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_HI();
    //tHIGH not less than 600ns
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    DELAY();
    SCL_TO_LOW();

    //release SDA for energy saving purpose
    SDA_TO_HI();
}

static uint8_t SW_I2C_Read_Data()
{
    uint8_t readdata = 0;
    //release SDA
    SDA_TO_HI();

    for (int x = 8; x--;)
    {
        //tLOW not less than 1,3us (but already included 100ns tSU_DAT)
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        SCL_TO_HI();
        //tHIGH not less than 600ns
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        DELAY();
        readdata <<= 1;
        if (GET_SDA())
            readdata |= 0x01;
        SCL_TO_LOW();
    }
    return readdata;
}

static uint8_t SW_I2C_Read_8addr(uint8_t IICID, uint8_t regaddr, uint8_t *pdata, uint8_t rcnt)
{
    uint8_t returnack = TRUE;
    uint8_t index;

    if (!rcnt) return FALSE;

    i2c_start_condition();
    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    i2c_register_address(regaddr);
    if (!i2c_check_ack()) { returnack = FALSE; }
    i2c_start_condition();                              //repeated START
    i2c_slave_address(IICID, READ_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    if (rcnt > 1)
    {
        for (index = 0; index < (rcnt - 1); index++)
        {
            pdata[index] = SW_I2C_Read_Data();
            i2c_send_ack(TRUE);
        }
    }
    pdata[rcnt - 1] = SW_I2C_Read_Data();
    i2c_send_ack(FALSE);
    i2c_stop_condition();

    return returnack;
}

/*
static uint8_t SW_I2C_Read_16addr(uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt)
{
    uint8_t returnack = TRUE;
    uint8_t index;

    if (!rcnt) return FALSE;

    i2c_port_initial();
    i2c_start_condition();
    //写ID
    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    //写高八位地址
    i2c_register_address((uint8_t)(regaddr >> 8));
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    //写低八位地址
    i2c_register_address((uint8_t)regaddr);
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    //重启I2C总线
    i2c_start_condition();
    //读ID
    i2c_slave_address(IICID, READ_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    //循环读数据
    if (rcnt > 1)
    {
        for (index = 0; index < (rcnt - 1); index++)
        {
            DELAY();
            pdata[index] = SW_I2C_Read_Data();
            i2c_send_ack();
        }
    }
    DelayUS(SW_I2C_WAIT_TIME);
    pdata[rcnt - 1] = SW_I2C_Read_Data();
    i2c_check_not_ack();
    i2c_stop_condition();

    return returnack;
}
*/

static uint8_t SW_I2C_Write_8addr(uint8_t IICID, uint8_t regaddr, const uint8_t *pdata, uint8_t rcnt)
{
    uint8_t returnack = TRUE;

    if (!rcnt) return FALSE;

    i2c_start_condition();
    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    i2c_register_address(regaddr);
    if (!i2c_check_ack()) { returnack = FALSE; }
    for (int index = 0; index < rcnt; index++)
    {
        i2c_register_address(pdata[index]);
        if (!i2c_check_ack()) { returnack = FALSE; }
    }
    i2c_stop_condition();
    return returnack;
}

/*
static uint8_t SW_I2C_Write_16addr(uint8_t IICID, uint16_t regaddr, uint8_t *pdata, uint8_t rcnt)
{
    uint8_t returnack = TRUE;

    if (!rcnt) return FALSE;

    i2c_port_initial();
    i2c_start_condition();
    //写ID
    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    //写高八位地址
    i2c_register_address((uint8_t)(regaddr >> 8));
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    //写低八位地址
    i2c_register_address((uint8_t)regaddr);
    if (!i2c_check_ack()) { returnack = FALSE; }
    DELAY();
    //写数据
    for (int index = 0; index < rcnt; index++)
    {
        SW_I2C_Write_Data(pdata[index]);
        if (!i2c_check_ack()) { returnack = FALSE; }
        DELAY();
    }
    i2c_stop_condition();
    return returnack;
}
*/


namespace HAL_I2C
{
    void Init()
    {
        gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_10 | GPIO_PIN_11);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_10 | GPIO_PIN_11);
    }

    bool Read(uint8 reg_addr, uint8 *reg_data, uint16 len)
    {
        return SW_I2C_Read_8addr((uint8)(I2C_SLAVE_ADDRESS7), reg_addr, reg_data, (uint8)len) == 1;
    }

    bool Write(uint8 reg_addr, uint8 *reg_data, int len)
    {
        return SW_I2C_Write_8addr((uint8)(I2C_SLAVE_ADDRESS7), reg_addr, reg_data, (uint8)len) == 1;
    }
}
