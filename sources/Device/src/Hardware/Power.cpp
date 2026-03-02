// 2024/03/20 23:04:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/Power.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Storage/Storage.h"
#include "Display/Display.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Display/MonochromeBitmap.h"
#include "Display/Pictures/1.bmp.inc"
#include "Display/Primitives.h"
#include "system.h"


namespace HAL_ADC
{
    extern float GetVoltage(bool force);
}


namespace Power
{
#ifdef MODEL7735
    static const int WIDTH = 38, HEIGHT = 14;
#else
    static const int WIDTH = 80, HEIGHT = 28;
#endif

    static PinIn pinCHRG(GPIOB, GPIO_PIN_3);

    static void PowerDown();

    static float voltage = 0.0f;
}


void Power::MeasVoltage()
{
    float sum = 0.0f;
    int num_meas = 10;

    for (int i = 0; i < num_meas; i++)
    {
        sum += HAL_ADC::GetVoltage(false);
    }

    voltage = sum / (float)num_meas;
}


void Power::Init()
{
    pinCHRG.Init();

#ifdef POWER_CONTROL_ENABLED

    MeasVoltage();

    if (voltage < 3.0f)
    {
        PCF8563::AlarmFlagEnable(false);

        PowerDown();
    }
    else if (voltage < 3.5f)
    {
        PCF8563::AlarmFlagEnable(false);

        Display::Init();

        TimeMeterMS meter;

        while (meter.ElapsedTime() < 3000)
        {
            Display::DrawLowVoltage();
        }

        PowerDown();
    }

    PCF8563::Update();

    if (PCF8563::IsAlarmed())
    {
        PCF8563::AlarmFlagEnable(false);
    }

    Timer::Delay(100);          // Антидребезг

#endif
}


void Power::Disable()
{
    while (Source::GetCountReceived())
    {
        Source::CancelFirst();
    }

    TimeMeterMS meter;

    while (meter.ElapsedTime() < 1500)
    {
        Display::DrawPowerOff();
    }

    PowerDown();
}


void Power::PowerDown()
{
    Storage::Save();

    PinOut pinPWR_CTRL(GPIOC, GPIO_PIN_13);
    pinPWR_CTRL.Init();

    pinPWR_CTRL.ToHi();

    while(true) { }
}


void Power::Update()
{
#ifdef POWER_CONTROL_ENABLED

    if (voltage <= 3.5f)
    {
        Disable();
    }

#endif
}


void Power::Draw()
{
#ifdef MODEL7735
    const int x = 121, y = 0;
    #define DRAW_SMALL Rect(5, 7).Fill(x - 4, y + 3)
#else
    const int x = Display::WIDTH - WIDTH - 2, y = 2;
    #define DRAW_SMALL Rect(10, 14).Fill(x - 8, y + 6)
#endif

    Color color = Color::GREEN;

    Rect(WIDTH, HEIGHT).Draw(x, y, color);

    DRAW_SMALL;

    if (voltage > 3.9f)        // Полный заряд
    {
        Rect(WIDTH, HEIGHT).Fill(x, y);
    }
    else if (voltage > 3.8f)        // Две трети заряда
    {
        int width = WIDTH * 2 / 3;

        Rect(width, HEIGHT).Fill(x + WIDTH - width, y);
    }
    else if (voltage > 3.7f)        // Одна треть заряда
    {
        int width = WIDTH / 3;

        Rect(width, HEIGHT).Fill(x + WIDTH - width, y, Color::YELLOW);
    }
    else if (voltage > 3.5f)        // Пустая батарея
    {
        Rect(WIDTH, HEIGHT).Draw(x, y, Color::RED);
        DRAW_SMALL;
    }

    if (voltage > 4.4f && pinCHRG.IsLow())
    {
        Color::RED.SetAsCurrent();

        MonochromeBitmap(bmp_zip_1).Draw(x, y, true);
    }
}
