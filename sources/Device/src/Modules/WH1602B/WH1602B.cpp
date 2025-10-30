// 2024/04/29 21:49:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/WH1602B/WH1602B.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include <gd32e23x.h>


namespace WH1602B
{
#define BUSY_CYCLE_TIME				(5u)	/* x 10us. See datasheet for minimal value. */
#define CLRSCR_CYCLE_TIME			(200u)	/* x 10us. See datasheet for minimal value. */
#define RETHOME_CYCLE_TIME			(200u)	/* x 10us. See datasheet for minimal value. */
#define INIT_CYCLE_TIME				(400u)
#define POWERUP_CYCLE_TIME			(2000u)

#define DISPLAY_CONFIG_4bit_2L_5x8		(0x28u) /* Use 4-bit interface, 2 or 4 Lines, 5x8 pixel resolution */
#define DISPLAY_CONFIG_4bit_1L_5x8		(0x20u) /* Use 4-bit interface, 1 Line, 5x8 pixel resolution */

#define VIEW_MODE_DispOn_BlkOn_CrsOn	(0x0Fu) /* Display On, Blink On, Cursor On */
#define VIEW_MODE_DispOn_BlkOff_CrsOn	(0x0Eu) /* Display On, Blink Off, Cursor On */
#define VIEW_MODE_DispOn_BlkOn_CrsOff	(0x0Du) /* Display On, Blink On, Cursor Off */
#define VIEW_MODE_DispOn_BlkOff_CrsOff	(0x0Cu) /* Display On, Blink Off, Cursor Off */
#define VIEW_MODE_DispOff_BlkOff_CrsOff	(0x08u) /* Display Off, Blink Off, Cursor Off */

#define ENTRY_MODE_DEC_NO_SHIFT			(0x04u)
#define ENTRY_MODE_DEC_WITH_SHIFT     	(0x05u)
#define ENTRY_MODE_INC_NO_SHIFT	      	(0x06u)
#define ENTRY_MODE_INC_WITH_SHIFT     	(0x07u)

#define DEFAULT_DISPLAY_CONFIG		DISPLAY_CONFIG_4bit_2L_5x8
#define DEFAULT_VIEW_MODE			VIEW_MODE_DispOn_BlkOff_CrsOff
#define DEFAULT_ENTRY_MODE			ENTRY_MODE_INC_NO_SHIFT


// PB1
#define SET_EN()    pinEN.ToHi()
#define CLR_EN()    pinEN.ToLow()
// PB0
#define SET_RS()    pinRS.ToHi()
#define CLR_RS()    pinRS.ToLow()
// PA4
#define SET_D7()    pinD7.ToHi()
#define CLR_D7()    pinD7.ToLow()
// PA5
#define SET_D6()    pinD6.ToHi()
#define CLR_D6()    pinD6.ToLow()
// PA6
#define SET_D5()    pinD5.ToHi()
#define CLR_D5()    pinD5.ToLow()
// PA7
#define SET_D4()    pinD4.ToHi()
#define CLR_D4()    pinD4.ToLow()
// PB2
#define SET_PWR()   pinPWR.ToHi()
#define CLR_PWR()   pinPWR.ToLow()

#define DATA_7_MASK     0x80u
#define DATA_6_MASK     0x40u
#define DATA_5_MASK     0x20u
#define DATA_4_MASK     0x10u
#define DATA_3_MASK     0x08u
#define DATA_2_MASK     0x04u
#define DATA_1_MASK     0x02u
#define DATA_0_MASK     0x01u

#define MCU_FREQ_VALUE (78000000 / 10000000U) //MCU clock frequency in MHz

#define ENABLE_CYCLE_TIME	1u	/* Minimal value ~ 1us */
#define AC_UPDATE_TIME		4u	/* Minimal value ~ 4us */

    static void lcdWrite(uint8 data);
    static void lcd10usDelay(volatile uint us);
    static void lcdHigh(uint8 data);
    static void lcdStrobe();
    static void lcdLow(uint8 data);

    static PinOut pinEN(GPIOB, GPIO_PIN_0);
    static PinOut pinRS(GPIOB, GPIO_PIN_1);
    static PinOut pinD7(GPIOA, GPIO_PIN_4);
    static PinOut pinD6(GPIOA, GPIO_PIN_5);
    static PinOut pinD5(GPIOA, GPIO_PIN_6);
    static PinOut pinD4(GPIOA, GPIO_PIN_7);
    static PinOut pinPWR(GPIOB, GPIO_PIN_2);

    static bool is_enabled = false;
    static uint time_enable = 0;
}


void WH1602B::Enable()
{
    if (is_enabled)
    {
        return;
    }

    is_enabled = true;

    pinPWR.Init();
    pinPWR.ToHi();

    pinEN.Init();
    pinRS.Init();
    pinD4.Init();
    pinD5.Init();
    pinD6.Init();
    pinD7.Init();

    lcd10usDelay(POWERUP_CYCLE_TIME);
    
    lcdWrite(0x02);
    lcd10usDelay(BUSY_CYCLE_TIME);
    lcdWrite(0x28);
    lcd10usDelay(BUSY_CYCLE_TIME);
    lcdWrite(0x0C);
    lcd10usDelay(BUSY_CYCLE_TIME);
    lcdWrite(0x01);
    lcd10usDelay(INIT_CYCLE_TIME);      //длительная задержка
    lcdWrite(0x06);
    lcd10usDelay(BUSY_CYCLE_TIME);
    lcdWrite(0x80);
    lcd10usDelay(BUSY_CYCLE_TIME);

    time_enable = TIME_MS;
}


void WH1602B::Disable()
{
    pinPWR.ToLow();
    pinEN.ToLow();
    pinRS.ToLow();
    pinD4.ToLow();
    pinD5.ToLow();
    pinD6.ToLow();
    pinD7.ToLow();

    is_enabled = false;
}


uint WH1602B::TimeEnabled()
{
    return TIME_MS - time_enable;
}


void WH1602B::WriteFull(uint8 buffer[32])
{
    CLR_RS();                   // Посылаем команду
    lcdWrite(0x80);

    SET_RS();                   // Посылаем данные

    for (int i = 0; i < 16; i++)
    {
        lcdWrite(buffer[i]);
    }

    CLR_RS();
    lcdWrite(0x80 | 0x40);

    SET_RS();

    for (int i = 16; i < 32; i++)
    {
        lcdWrite(buffer[i]);
    }
}


void WH1602B::LoadSymbolToCGRAM(int slot, const uint8 rows[7])
{
    CLR_RS();
    lcdWrite((uint8)(0x40 | (slot * 8)));

    SET_RS();

    for (int i = 0; i < 7; i++)
    {
        lcdWrite(rows[i]);
    }
}


void WH1602B::Clear()
{
    CLR_RS();

    lcdWrite(0x01u);

    lcd10usDelay(CLRSCR_CYCLE_TIME);
}


void WH1602B::lcdWrite(uint8 data)
{
    lcdHigh(data);
    lcdStrobe();
    lcdLow(data);
    lcdStrobe();
    /* The busy flag must be checked after the 4-bit data has been transferred twice. */
    lcd10usDelay(200);
}


void WH1602B::lcd10usDelay(volatile uint us)
{
    us = 500;

    while (us > 0u)
    {
        us--;
    }
}


void WH1602B::lcdStrobe(void)
{/* Low level function. */
    SET_EN();
    lcd10usDelay(ENABLE_CYCLE_TIME);
    CLR_EN();
    lcd10usDelay(100);
}


void WH1602B::lcdLow(uint8 data)
{/* Low level function. */
    if (data & DATA_3_MASK) SET_D7(); else CLR_D7();
    if (data & DATA_2_MASK) SET_D6(); else CLR_D6();
    if (data & DATA_1_MASK) SET_D5(); else CLR_D5();
    if (data & DATA_0_MASK) SET_D4(); else CLR_D4();
}


void WH1602B::lcdHigh(uint8 data)
{/* Low level function. */
    if (data & DATA_7_MASK) SET_D7(); else CLR_D7();
    if (data & DATA_6_MASK) SET_D6(); else CLR_D6();
    if (data & DATA_5_MASK) SET_D5(); else CLR_D5();
    if (data & DATA_4_MASK) SET_D4(); else CLR_D4();
}
