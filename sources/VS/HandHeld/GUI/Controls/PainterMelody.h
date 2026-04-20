// 2024/03/29 12:06:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "GUI/Controls/Painter.h"


class PainterMelody : public Painter
{
public:

    static void Create(wxWindow *parent, const wxSize &_size);

    static PainterMelody *self;

    void EnableMelody(int);

    void DisableMelody();

    virtual void Update() override;

private:

    PainterMelody(wxWindow *parent, const wxSize &_size) :
        Painter(parent, _size, 1) { };

    int melody = -1;
};
