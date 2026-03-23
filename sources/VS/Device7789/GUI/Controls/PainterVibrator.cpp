// 2024/03/29 12:07:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines_win.h"
#include "GUI/Controls/PainterVibrator.h"


PainterVibrator *PainterVibrator::self = nullptr;


void PainterVibrator::Create(wxWindow *parent, const wxSize &_size)
{
    self = new PainterVibrator(parent, _size);
}


void PainterVibrator::SetEnabled()
{
    enabled = true;

    radius = min;

    delta = 1;
}


void PainterVibrator::SetDisabled()
{
    enabled = false;
}


void PainterVibrator::Update()
{
    if (!enabled)
    {
        return;
    }

    BeginScene();

    memDC.SetBrush(*wxBLACK_BRUSH);

    memDC.Clear();

    memDC.SetBrush(*wxYELLOW_BRUSH);
    memDC.SetPen(*wxYELLOW_PEN);

    memDC.DrawCircle(15, 15, radius);

    radius += delta;

    if (radius > max || radius < min)
    {
        delta = -delta;
    }

    EndScene();
}
