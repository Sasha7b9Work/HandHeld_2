// 2024/03/06 14:37:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/PCF8563/PCF8563.h"


namespace PCF8563
{
    static RTCDateTime time =
    {
        6,
        5,
        4,
        1,
        2,
        3
    };
}


void PCF8563::Init()
{

}


void PCF8563::Update()
{

}


void PCF8563::SetTimer(uint /*time_sec*/)
{

}


RTCDateTime PCF8563::GetDateTime()
{
    wxDateTime data_time = wxDateTime::Now();

    return
    {
        (uint8)(data_time.GetYear() - 2000),
        (uint8)(data_time.GetMonth() + 1),
        (uint8)(data_time.GetDay()),
        (uint8)(data_time.GetHour()),
        (uint8)(data_time.GetMinute()),
        (uint8)(data_time.GetSecond())
    };
}


void PCF8563::SetDateTime(RTCDateTime *date_time)
{
    time = *date_time;
}


void PCF8563::SetAlarm(RTCDateTime *, bool)
{

}


void PCF8563::AlarmFlagEnable(bool)
{

}


bool PCF8563::IsAlarmed()
{
    return false;
}


void PCF8563::AlarmInterruptEnable(bool)
{

}


void PCF8563::DisableAlarm()
{

}
