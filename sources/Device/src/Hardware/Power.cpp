// 2024/03/20 23:04:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Hardware/Power.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Modules/LED/driverLED.h"
#include "Storage/Storage.h"
#ifdef TYPE_1602
    #include "Display/Display1602.h"
#else
    #include "Display/Display7735.h"
#endif


namespace Power
{
#ifdef TYPE_1602
#else
    static const int WIDTH = 38;
    static const int HEIGHT = 14;
#endif

    static void PowerDown();
}


void Power::Init()
{
    pinPWR.Init();
    pinPWR_CTRL.Init();

    pinPWR.ToHi();

    float voltage = HAL_ADC::GetVoltage(true);

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
    else
    {
        while (pinPWR_CTRL.IsLow())
        {
            Display::DrawPowerOn();
        }
    }

    Timer::Delay(100);          // �����������
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

    pinPWR.ToLow();

    LED::Driver::On();

    while(true) { }
}


void Power::Update()
{
    if (HAL_ADC::GetVoltage(false) <= 3.5f)
    {
        Disable();
    }

    if (pinPWR_CTRL.IsLow())
    {
        PCF8563::Update();

        if (!PCF8563::IsAlarmed())
        {
            Disable();
        }
        else
        {
            PCF8563::AlarmFlagEnable(false);
        }
    }
}


void Power::Draw()
{
    float voltage = HAL_ADC::GetVoltage(false);

#ifdef TYPE_1602

    if (voltage > 3.9f)         // 5/5 ������ �����
    {
        Display::DrawSymbol(15, 0, 0xF5);
    }
    else if (voltage > 3.85f)
    {
        Display::DrawSymbol(15, 0, 0xF4);
    }
    else if (voltage > 3.8f)
    {
        Display::DrawSymbol(15, 0, 0xF3);
    }
    else if (voltage > 3.7f)
    {
        Display::DrawSymbol(15, 0, 0xF2);
    }
    else if (voltage > 3.6f)
    {
        Display::DrawSymbol(15, 0, 0xF1);
    }
    else if (voltage > 3.5f)
    {
        Display::DrawSymbol(15, 0, 0xF0);
    }

#else
    int x = 121;
    int y = 0;

    Color color = Color::GREEN;

    Rect(WIDTH, HEIGHT).Draw(x, y, color);
    Rect(5, 7).Fill(x - 4, y + 3);

    if (voltage > 3.9f)        // ������ �����
    {
        Rect(WIDTH, HEIGHT).Fill(x, y);
    }
    else if (voltage > 3.8f)        // ��� ����� ������
    {
        int width = WIDTH * 2 / 3;

        Rect(width, HEIGHT).Fill(x + WIDTH - width, y);
    }
    else if (voltage > 3.7f)        // ���� ����� ������
    {
        int width = WIDTH / 3;

        Rect(width, HEIGHT).Fill(x + WIDTH - width, y, Color::YELLOW);
    }
    else if (voltage > 3.5f)        // ������ �������
    {
        Rect(WIDTH, HEIGHT).Draw(x, y, Color::RED);
        Rect(5, 7).Fill(x - 4, y + 3);
    }
#endif
}
