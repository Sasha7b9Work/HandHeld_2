// 2024/03/20 23:04:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/Power.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Storage/Storage.h"
#include "Display/Display.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "system.h"


namespace Power
{
    static const int WIDTH = 38;
    static const int HEIGHT = 14;

    static PinOut pinCHRG(GPIOB, GPIO_PIN_3);

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
#ifdef POWER_CONTROL_ENABLED

    voltage = HAL_ADC::GetVoltage(true);

    if (voltage < 3.0f)
    {
        PCF8563::AlarmFlagEnable(false);

        PowerDown();
    }
    else if (voltage < 3.5f)
    {
        PCF8563::AlarmFlagEnable(false);

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
    int x = 121;
    int y = 0;

    Color color = Color::GREEN;

    Rect(WIDTH, HEIGHT).Draw(x, y, color);
    Rect(5, 7).Fill(x - 4, y + 3);

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
        Rect(5, 7).Fill(x - 4, y + 3);
    }
}
