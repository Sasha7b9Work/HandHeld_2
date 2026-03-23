// 2024/03/29 11:28:50 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines_win.h"
#include "GUI/Controls/Painter.h"


wxSize Painter::size_indicator(30, 30);


Painter::Painter(wxWindow *parent, const wxSize &_size, int _scale) :
    wxPanel(parent), size(_size), scale(_scale)
{
    bitmap = new wxBitmap(size);

    SetMinSize(size * scale);
    SetSize(size * scale);
    SetDoubleBuffered(true);
    Bind(wxEVT_PAINT, &Painter::OnPaint, this);
}


void Painter::OnPaint(wxPaintEvent &)
{
    wxPaintDC dc(this);

    wxImage image = bitmap->ConvertToImage().Rescale(size.x * scale, size.y * scale);

//    wxImage image = bitmap->ConvertToImage();

    dc.DrawBitmap(wxBitmap(image), 0, 0);
}


void Painter::BeginScene()
{
    memDC.SelectObject(*bitmap);
}


void Painter::EndScene()
{
    memDC.SelectObject(wxNullBitmap);

    Refresh();
}
