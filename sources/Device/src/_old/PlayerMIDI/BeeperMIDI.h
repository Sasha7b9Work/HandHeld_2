// 2024/03/03 17:06:14 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace BeeperMIDI
{
    void Init();

    // Вызывается плеером, когда игрок мелодия стартует (из StartMelody()). Может использоваться для настройки таймера/ШИМ.
    void CallbackOnStartMelody();

    // Вывод отсчёта
    void CallbackOnOutputSample(uint8);

    // Вызывается проигрывателем после завершения мелодии (из TimerFunc(), прерывания отключены). Может использоваться для настройки таймера/ШИМ.
    void CallbackOnStopMelody();
}
