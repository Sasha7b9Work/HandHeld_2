#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <stdlib.h>
#include "system.h"

#ifdef WIN32
    #define __asm(x)
#endif


#define SW_I2C_WAIT_TIME    2 // 10us 100kHz

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


#ifdef MODEL7735
    #define PORT_I2C  GPIOA
    #define I2C_SCK   GPIO_PIN_0
    #define I2C_SDA   GPIO_PIN_1
#endif

#ifdef MODEL7789
    #define PORT_I2C  GPIOB
    #define I2C_SCK   GPIO_PIN_6
    #define I2C_SDA   GPIO_PIN_7
#endif


#define SDA_TO_LOW()    GPIO_BC(PORT_I2C) = (uint)I2C_SDA
#define SDA_TO_HI()     GPIO_BOP(PORT_I2C) = (uint)I2C_SDA

#define GET_SDA()       (GPIO_ISTAT(PORT_I2C)&(I2C_SDA))

#define SCL_TO_LOW()    GPIO_BC(PORT_I2C) = (uint)I2C_SCK
#define SCL_TO_HI()     GPIO_BOP(PORT_I2C) = (uint)I2C_SCK

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
    if (!i2c_check_ack()) { returnack = FALSE; } //-V1048
    i2c_start_condition();                              //repeated START
    i2c_slave_address(IICID, READ_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; } //-V1048
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


static uint8_t SW_I2C_Write_8addr(uint8_t IICID, uint8_t regaddr, const uint8_t *pdata, uint8_t rcnt)
{
    uint8_t returnack = TRUE;

    if (!rcnt) return FALSE;

    i2c_start_condition();
    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_ack()) { returnack = FALSE; }
    i2c_register_address(regaddr);
    if (!i2c_check_ack()) { returnack = FALSE; } //-V1048
    for (int index = 0; index < rcnt; index++)
    {
        i2c_register_address(pdata[index]);
        if (!i2c_check_ack()) { returnack = FALSE; }
    }
    i2c_stop_condition();
    return returnack;
}


namespace HAL_I2C
{
    void Init()
    {
#ifdef MODEL7735
        gpio_mode_set(PORT_I2C, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, I2C_SCK | I2C_SDA);
        gpio_output_options_set(PORT_I2C, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCK | I2C_SDA);
#endif

#ifdef MODEL7789
        gpio_init(PORT_I2C, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, I2C_SCK | I2C_SDA);
#endif
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
