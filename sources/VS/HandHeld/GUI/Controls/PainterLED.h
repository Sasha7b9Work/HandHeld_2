// 2024/03/29 12:06:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "GUI/Controls/Painter.h"


class PainterLED : public Painter
{
public:

    static void Create(wxWindow *parent, const wxSize &);

    static PainterLED *self;

    void SetEnabled();

    void SetDisabled();

    bool IsEnabled() const { return is_enabled; };

private:

    PainterLED(wxWindow *parent, const wxSize &_size) :
        Painter(parent, _size, 1) { };

    bool is_enabled = false;
};
