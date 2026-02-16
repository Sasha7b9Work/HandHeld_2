// 2022/04/27 11:11:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines_win.h"
#include "Frame.h"
#include "Display/Display7735.h"
#include "Modules/ST7735/ST7735.h"
#include "Application.h"
#include "Menu/Menu.h"
#include "wx/statline.h"
#include "Keyboard/Keyboard.h"
#include "Settings/Source.h"
#include "Modules/CMT2210AW/EmulatorReceiver.h"
#include "GUI/Controls/Painter.h"
#include "GUI/Controls/PainterVibrator.h"
#include "Hardware/Timer.h"


namespace Keyboard
{
    void AppendAction(const Action &);
}


Frame *Frame::self = nullptr;

enum
{
    TIMER_ID = 111,
    TIMER_BUTTON_ID
};


static wxColour ConvertColor(Color::E e)
{
    uint16 value = Color::colors[e];

    float b = (float)BLUE_FROM_COLOR(value);
    float g = (float)GREEN_FROM_COLOR(value);
    float r = (float)RED_FROM_COLOR(value);

    uint8 blue = (uint8)((b / 31.0f) * 255);
    uint8 green = (uint8)((g / 63.0f) * 255);
    uint8 red = (uint8)((r / 31.0f) * 255);

    return wxColour(red, green, blue);
}


class Screen : public Painter
{
public:
    Screen(wxWindow *parent) :
        Painter(parent, { Display::WIDTH, Display::HEIGHT }, IMAGE_SCALE)
    {
    }

    void WriteBuffer(int y0)
    {
        static const wxColour colors[Color::Count] =
        {
            ConvertColor((Color::E)0),
            ConvertColor((Color::E)1),
            ConvertColor((Color::E)2),
            ConvertColor((Color::E)3),
            ConvertColor((Color::E)4),
            ConvertColor((Color::E)5),
            ConvertColor((Color::E)6),
            ConvertColor((Color::E)7),
            ConvertColor((Color::E)8),
            ConvertColor((Color::E)9)
        };

        BeginScene();

        wxPen pen = *wxWHITE_PEN;

        int line = 0;

        for (int y = y0; y < y0 + Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT; y++)
        {
            uint8 *points = Display::Buffer::GetLine(line++);

            uint8 value = *points;

            for (int x = 0; x < Display::WIDTH; x++)
            {
                pen.SetColour(colors[value]);

                memDC.SetPen(pen);

                memDC.DrawPoint(x + 1, y);

                value = *(++points);
            }
        }

        EndScene();
    }
};


static Screen *screen = nullptr;


Frame::Frame(const wxString &title)
    : wxFrame((wxFrame *)NULL, wxID_ANY, title)
{
    SetIcon(wxICON(sample));

    self = this;

    CreateMenu();

    screen = new Screen(this);

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(screen);
    SetSizer(sizer);

    Bind(wxEVT_PAINT, &Frame::OnPaint, this);
    Bind(wxEVT_TIMER, &Frame::OnTimer, this, TIMER_ID);

    timerButton.SetOwner(this, TIMER_BUTTON_ID);
    timer.SetOwner(this, TIMER_ID);

    SetClientSize(Display::WIDTH * IMAGE_SCALE + 300, Display::HEIGHT * IMAGE_SCALE);

    wxTopLevelWindowBase::SetMinSize(GetSize());
    wxTopLevelWindowBase::SetMaxSize(GetSize());

    const int maxWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    const int maxHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

    int x = (maxWidth - GetSize().x) / 2;
    int y = (maxHeight - GetSize().y) / 2;

    SetPosition({ x, y });

    const int dy = 10;

    int x1 = 330;
    int x2 = 355;
    int x3 = 380;
    int y1 = 15 + dy;
    int y2 = 50 + dy;
    int y3 = 85 + dy;

    CreateButton(ID_BUTTON_MENU, "Menu", { x1, y2 }, 45);

    CreateButton(ID_BUTTON_CANCEL, "Cancel", { x3, y2 }, 45);

    CreateButton(ID_BUTTON_UP, "Up", { x2, y1 }, 45);

    CreateButton(ID_BUTTON_DOWN, "Down", { x2, y3 }, 45);

    pchar names[Source::Count] =
    {
        "Звонок",
        "Мобильный",
        "Телефон",
        "Домофон",
        "Датчик"
    };

    for (int i = 0; i < 5; i++)
    {
        CreateButton(ID_BUTTON_SIGNAL_1 + i, names[i], { 440, 5 + i * 30 }, 90);
    }

    Bind(wxEVT_CLOSE_WINDOW, &Frame::OnCloseWindow, this);

    x = 550;

    wxSize size_label(50, 20);

    const int x_label = 540;

    new wxStaticText(this, wxID_ANY, _("Звук"), { x_label, 0 }, size_label, wxALIGN_CENTRE_HORIZONTAL);

    new wxStaticText(this, wxID_ANY, _("LED"), { x_label, 50 }, size_label, wxALIGN_CENTRE_HORIZONTAL);

    new wxStaticText(this, wxID_ANY, _("Вибро"), { x_label, 100 }, size_label, wxALIGN_CENTRE_HORIZONTAL);

    PainterVibrator::Create(this, Painter::size_indicator);

    PainterVibrator::self->SetPosition({ x, 120 });

    timer.StartOnce(25);
}


void Frame::CreateButton(int id, pchar title, const wxPoint &coord, int width)
{
    const wxSize SIZE_BUTTON = { width, 25 };

    wxButton *button = new wxButton(this, id, title, coord , SIZE_BUTTON);

    button->Connect(id, wxEVT_LEFT_DOWN, wxCommandEventHandler(Frame::OnButtonDownEvent));
    button->Connect(id, wxEVT_LEFT_UP, wxCommandEventHandler(Frame::OnButtonUpEvent));
    button->Connect(id, wxEVT_BUTTON, wxCommandEventHandler(Frame::OnButtonEvent));
} //-V773


void Frame::CreateMenu()
{
    wxMenuBar *menuBar = new wxMenuBar;

    wxMenu *menuFile = new wxMenu;

    menuBar->Append(menuFile, _("Файл"));

    wxFrameBase::SetMenuBar(menuBar);
}


void Frame::OnTimer(wxTimerEvent &)
{
    timer.Stop();

    Application::Self()->Update();

    timer.Start(25);
}


void Frame::OnButtonEvent(wxCommandEvent &event)
{
    EmuRecv::EmulateSignal((Source::E)(event.GetId() - ID_BUTTON_SIGNAL_1));

    event.Skip();
}


void Frame::OnButtonDownEvent(wxCommandEvent &event)
{
    int id = event.GetId();

    Action action;

    action.type = ActionType::Down;

    bool execute = false;

    if (id == ID_BUTTON_MENU)
    {
        action.key = Key::Menu;
        execute = true;
    }
    else if (id == ID_BUTTON_CANCEL)
    {
        action.key = Key::Cancel;
        execute = true;
    }
    else if (id == ID_BUTTON_UP)
    {
        action.key = Key::Up;
        execute = true;
    }
    else if (id == ID_BUTTON_DOWN)
    {
        action.key = Key::Down;
        execute = true;
    }

    if (execute)
    {
        Keyboard::AppendAction(action);
    }

    event.Skip();
}


void Frame::OnButtonUpEvent(wxCommandEvent &event)
{
    int id = event.GetId();

    Action action;

    action.type = ActionType::Up;

    bool execute = false;

    if (id == ID_BUTTON_MENU)
    {
        action.key = Key::Menu;
        execute = true;
    }
    else if (id == ID_BUTTON_CANCEL)
    {
        action.key = Key::Cancel;
        execute = true;
    }
    else if (id == ID_BUTTON_UP)
    {
        action.key = Key::Up;
        execute = true;
    }
    else if (id == ID_BUTTON_DOWN)
    {
        action.key = Key::Down;
        execute = true;
    }

    if (execute)
    {
        Keyboard::AppendAction(action);
    }

    event.Skip();
}


void Frame::OnCloseWindow(wxCloseEvent &event)
{
    event.Skip();
}


void ST7735::Init()
{

}


void ST7735::WriteBuffer(int y0)
{
    screen->WriteBuffer(y0);
}


void ST7735::Enable()
{

}


void ST7735::Disable()
{

}


uint ST7735::TimeEnabled()
{
    return TIME_MS;
}
