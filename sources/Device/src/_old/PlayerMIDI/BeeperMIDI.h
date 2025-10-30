// 2024/03/03 17:06:14 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace BeeperMIDI
{
    void Init();

    // ���������� �������, ����� ����� ������� �������� (�� StartMelody()). ����� �������������� ��� ��������� �������/���.
    void CallbackOnStartMelody();

    // ����� �������
    void CallbackOnOutputSample(uint8);

    // ���������� �������������� ����� ���������� ������� (�� TimerFunc(), ���������� ���������). ����� �������������� ��� ��������� �������/���.
    void CallbackOnStopMelody();
}
