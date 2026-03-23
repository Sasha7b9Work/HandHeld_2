// 2024/03/23 13:37:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


struct Icon
{
    void Draw(int pos_on_display) const;

    virtual void Draw(int x, int y) const = 0;
};


struct IconSourceDoorBell : public Icon
{
    virtual void Draw(int x, int y) const override;
};


struct IconSourceMobile : public Icon
{
    virtual void Draw(int x, int y) const override;
};


struct IconSourcePhone : public Icon
{
    virtual void Draw(int x, int y) const override;
};


struct IconSourceIntercom : public Icon
{
    virtual void Draw(int x, int y) const override;
};


struct IconSourceSensor : public Icon
{
    virtual void Draw(int x, int y) const override;
};


extern Icon *sourceIcons[Source::Count];
