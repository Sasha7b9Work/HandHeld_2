// 2024/03/01 22:29:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Keyboard/Keyboard.h"
#include "Menu/Menu.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Hardware/Vibrato.h"
#include "Storage/Storage.h"
#include "Modules/LED/LED.h"
#include "Modules/Beeper/Beeper.h"
#include "Modules/ST7735/ST7735.h"
#include "Hardware/Power.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Display/Display7735.h"
#include "Modules/PAN3060/PAN3060.h"


/*
    * при разряде батареи ниже допустимого предела тоже выключение
    * будильник надо проверить
    * Включение.  надо выяснить, что послужило включением, был ли это будильник, если нет,
      то ничего не нужно, просто включились и всё. Ну и пин PWR (PA12) надо поднять
    * Выключение : если PWR_CTRL(PA0) перешёл в 0, то это одно
      из двух : либо сработал INT от мсх часов(будильник), либо нажали кнопку выключения.
      Надо проверить регистры часов и если там будильник, включить его индикацию(на часах
      его тоже сбросить, конечно), а если нет, то написать на экране ВЫКЛЮЧЕНИЕ, сделать
      задержку 1, 5с и снять единицу с ноги PWR(PA12).
*/


void Device::Init()
{
    HAL::Init();

    PCF8563::Init();

    Display::Init();

//    Power::Init();

    Timer::Init();

    Keyboard::Init();

    Menu::Init();

    LED::Init();

    Vibrato::Init();

    ModeClock::Set(ModeClock::Hi);

    uint clock = SystemCoreClock;
    clock = clock;

    PAN3060::Init();

    Beeper::Init();

    Storage::Init();
}

void Device::Update2()
{
//    if (!Beeper::IsRunning() && Beeper::TimeAfterStop() > 5000)
//    {
//        Beeper::Play(TypeSound::_1, 2);
//    }

    Beeper::Play(TypeSound::_4, 2);


//    Display::Update2();

    Beeper::Update();

    PAN3060::Update();
}


void Device::Update()
{
    bool recv_enabled = PAN3060::IsEnabled();
    bool keyboard_more_time = Keyboard::ToMoreTime();
    int num_sources = Source::GetCountReceived();
    bool is_alarmed = PCF8563::IsAlarmed();

    if (!recv_enabled && keyboard_more_time && num_sources == 0 && !is_alarmed)
    {
        ModeClock::Set(ModeClock::DeepSleep);
    }

    ModeClock::LeaveDeepSleep();

    PCF8563::Update();

    Menu::Update();

    PAN3060::Update();

    if (Source::GetCountReceived() || !Keyboard::ToMoreTime())
    {
        Display::Update();
    }

    LED::Update();

    Vibrato::Update();

    Beeper::Update();

    Source::Update();

    Power::Update();
}
