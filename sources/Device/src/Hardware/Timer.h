// 2024/03/02 09:30:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#ifdef WIN32
    #define TIME_MS Timer::CurrentTime()
#else
    #define TIME_MS timer_counter
#endif


namespace Timer
{
//    void Init();

    void Delay(uint timeMS);

    uint CurrentTime();
}


//namespace TimerUS
//{
//    void Reset();
//
//    uint ElaplsedTime();
//}


struct TimeMeterMS
{
    TimeMeterMS()
    {
        Reset();
    }

    void Reset();

    uint ElapsedTime() const;

    void Pause();

    void Resume();

private:

    uint time_reset;

    uint time_begin_pause;

    bool in_pause = false;
};


extern uint timer_counter;
