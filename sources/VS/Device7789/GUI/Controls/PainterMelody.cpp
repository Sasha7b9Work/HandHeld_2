// 2024/03/29 12:07:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines_win.h"
#include "GUI/Controls/PainterMelody.h"


PainterMelody *PainterMelody::self = nullptr;


void PainterMelody::Create(wxWindow *parent, const wxSize &_size)
{
    self = new PainterMelody(parent, _size);
}


void PainterMelody::EnableMelody(int number)
{
    melody = number;
}


void PainterMelody::DisableMelody()
{
    melody = -1;

    BeginScene();

    memDC.SetBrush(*wxBLACK_BRUSH);

    memDC.Clear();

    EndScene();
}


void PainterMelody::Update()
{
    if (melody < 0)
    {
        return;
    }

    BeginScene();

    wxPen pen = *wxBLUE_PEN;

    wxBrush brush = *wxBLUE_BRUSH;

    wxFont font = wxFont(wxFontInfo(12).Family(wxFONTFAMILY_ROMAN));

    memDC.DrawText("T", 1, 20);

    for (int i = 0; i < melody + 1; i++)
    {
        wxColour color = wxColour(rand() % 255, rand() % 255, 255);

        pen.SetColour(color);
        brush.SetColour(color);

        memDC.SetPen(pen);
        memDC.SetBrush(brush);
        memDC.SetFont(font);

        memDC.DrawRectangle(3 + i * 5, 5, 4, 20);
    }

    EndScene();
}
