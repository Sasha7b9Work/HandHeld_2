// 2024/03/01 22:47:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace ST7735_89
{
    void BeforeInit();

    void Init();

    // Записывает буфер дисплея в строки, начиная с y
    void WriteBuffer(int num_part);

    void Enable();

    void Disable();

    bool IsEnabled();

    uint TimeEnabled();

    // Вызывается по выходу из спящего режима
    void EnablePins();
}
