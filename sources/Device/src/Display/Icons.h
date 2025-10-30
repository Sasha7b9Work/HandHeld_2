// 2024/03/23 13:37:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


struct Icon
{
    virtual void Draw(int x, int y, const Color &) const;
};


struct IconSourceDoorBell : public Icon
{
    virtual void Draw(int x, int y, const Color &) const override;
};


struct IconSourceMobile : public Icon
{
    virtual void Draw(int x, int y, const Color &) const override;
};


struct IconSourcePhone : public Icon
{
    virtual void Draw(int x, int y, const Color &) const override;
};


struct IconSourceIntercom : public Icon
{
    virtual void Draw(int x, int y, const Color &) const override;
};


struct IconSourceSensor : public Icon
{
    virtual void Draw(int x, int y, const Color &) const override;
};


#ifndef TYPE_1602
extern Icon *sourceIcons[Source::Count];
#endif
