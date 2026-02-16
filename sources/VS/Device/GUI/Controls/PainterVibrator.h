// 2024/03/29 12:06:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "GUI/Controls/Painter.h"


class PainterVibrator : public Painter
{
public:

    static void Create(wxWindow *parent, const wxSize &);

    static PainterVibrator *self;

    void SetEnabled();

    void SetDisabled();

    virtual void Update() override;

private:

    PainterVibrator(wxWindow *parent, const wxSize &_size) :
        Painter(parent, _size, 1) { };

    bool enabled = false;

    int radius = min;
    int delta = 1;
    static const int min = 8;
    static const int max = 10;
};
