// 2024/03/01 22:58:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Menu/MenuItems.h"


namespace Menu
{
    void Init();

    void Update();

    // ���������� �� ����
    bool IsShown();

    void Draw();

    void Close();

    void SetCurrentItem(const Item *);
}
