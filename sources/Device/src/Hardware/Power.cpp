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
#include "Display/Primitives.h"
#include "Modules/LED/LED.h"
#include "Hardware/Vibrato.h"
#include "Modules/Beeper/Beeper.h"
#include "system.h"
#ifdef MODEL7735
    #include "Display/Pictures/1.bmp.inc"
#else
    #include "Display/Pictures/1_7789.bmp.inc"
#endif


namespace HAL_ADC
{
    extern float GetVoltage();
}


namespace Display
{
    namespace Buffer
    {
        extern int current_part;
    }
}


namespace Power
{
    static PinIn pinCHRG(GPIOB, GPIO_PIN_3);

    static void PowerDown();

#ifdef NEED_TO_CHANGE_VOLTAGE_MEASUREMENTS
    uint time_control_ms = 1000;
#else
    uint time_control_ms = 60000;
#endif

    // Контролировать ли напряжение питания (при подключенном отладчике напряжение питания определяется неправильно)
    static bool PowerControlEnabled()
    {
        if (HAL::IsDebugBoard())
        {
#ifdef POWER_CONTROL_DISABLE
            return false;
#else
            return true;
#endif
        }

        return true;
    }
}


float Power::MeasVoltage(bool after_sleep)
{
    static float voltage = 10.0f;

    if (after_sleep)
    {
        voltage = 10.0f;
    }

    static TimeMeterMS meter;

    if (meter.ElapsedTime() > time_control_ms || voltage > 9.0f)
    {
        if (!LED::IsFired() &&
            !Vibrato::IsRunning() &&
            !Beeper::IsRunning() &&
            Source::GetCountReceived() == 0)
        {
            meter.Reset();

            float sum = 0.0f;
            const int num_meas = 10;

            for (int i = 0; i < num_meas; i++)
            {
                sum += HAL_ADC::GetVoltage();
            }

            voltage = sum / (float)num_meas;
        }
    }

    return (voltage > 8.0f) ? 5.0f : voltage;
}


void Power::Init()
{
    pinCHRG.Init();

    if (!PowerControlEnabled())
    {
        return;
    }

    if (MeasVoltage(false) < 3.0f)
    {
        PCF8563::AlarmFlagEnable(false);

        PowerDown();
    }
    else if (MeasVoltage(false) < 3.5f)
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
}


void Power::Disable()
{
    if (!PowerControlEnabled())
    {
        return;
    }

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
    static int counter = 0;

    if (MeasVoltage(false) <= 3.5f)
    {
        counter++;
    }
    else
    {
        counter = 0;
    }

    if (counter > 5)
    {
        Disable();
    }
}


void Power::Draw()
{
#ifdef MODEL7735
    if (Display::Buffer::current_part > 1)
    {
        return;
    }

#ifdef NEED_TO_CHANGE_VOLTAGE_MEASUREMENTS
    Font::StoreType();

    Font::SetSmallType();

    Color::WHITE.SetAsCurrent();

    Text<>("%.2f В", (double)MeasVoltage(false)).Write(0, 0);

    Font::RestoreType();
#endif

    static const int WIDTH = 38, HEIGHT = 14;
    const int x = 121, y = 0;
    #define DRAW_SMALL Rect(5, 7).Fill(x - 4, y + 3)
#else
    if (Display::Buffer::current_part > 0)
    {
        return;
    }
    static const int WIDTH = 80, HEIGHT = 28;
    const int x = Display::WIDTH - WIDTH - 2, y = 2;
    #define DRAW_SMALL Rect(10, 14).Fill(x - 9, y + 6)
#endif

    Color color = Color::GREEN;

    Rect(WIDTH, HEIGHT).Draw(x, y, color);

    DRAW_SMALL;

    if (MeasVoltage(false) > 3.9f)        // Полный заряд
    {
        Rect(WIDTH, HEIGHT).Fill(x, y);
    }
    else if (MeasVoltage(false) > 3.8f)        // Две трети заряда
    {
        int width = WIDTH * 2 / 3;

        Rect(width, HEIGHT).Fill(x + WIDTH - width, y);
    }
    else if (MeasVoltage(false) > 3.7f)        // Одна треть заряда
    {
        int width = WIDTH / 3;

        Rect(width, HEIGHT).Fill(x + WIDTH - width, y, Color::YELLOW);
    }
    else if (MeasVoltage(false) > 3.5f)        // Пустая батарея
    {
        Rect(WIDTH, HEIGHT).Draw(x, y, Color::RED);
    }

    if (MeasVoltage(false) > 4.4f && pinCHRG.IsLow())
    {
        Color::RED.SetAsCurrent();

#ifdef MODEL7735
        MonochromeBitmap(bmp_zip_1).Draw(x, y, true);
#else
        MonochromeBitmap(bmp_zip_1_7789).Draw(x, y, false);
#endif
    }
}
