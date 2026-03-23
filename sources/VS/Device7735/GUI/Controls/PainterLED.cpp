// 2024/03/29 12:07:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines_win.h"
#include "GUI/Controls/PainterLED.h"


PainterLED *PainterLED::self = nullptr;


void PainterLED::Create(wxWindow *parent, const wxSize &_size)
{
    self = new PainterLED(parent, _size);
}


void PainterLED::SetEnabled()
{
    is_enabled = true;

    BeginScene();

    memDC.SetPen(*wxYELLOW_PEN);
    memDC.SetBrush(*wxYELLOW_BRUSH);

    memDC.DrawLine(0, 0, 30, 30);
    memDC.DrawLine(0, 30, 30, 0);

    EndScene();
}


void PainterLED::SetDisabled()
{
    is_enabled = false;

    BeginScene();

    memDC.SetPen(*wxBLACK_PEN);
    memDC.SetBrush(*wxBLACK_BRUSH);

    memDC.DrawRectangle(0, 0, GetSize().x, GetSize().y);

    EndScene();
}
