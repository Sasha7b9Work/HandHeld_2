// 2024/03/01 22:46:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace PAN3060
{
    void Init();

    void Update();

    void FuncDraw();

    void CallbackOnIRQ();

    // Идёт процесс обновления
    bool InProcessUpgrade();
}
