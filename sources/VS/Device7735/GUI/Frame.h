// 2022/04/27 11:12:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/Timer.h"



class Frame : public wxFrame
{
public:
    Frame(const wxString &title);

    static Frame *self;

    void OnTimer(wxTimerEvent &);

private:

    wxTimer timer;

    TimeMeterMS meterButton;        // Для кнопки
    wxTimer timerButton;

    void OnButtonDownEvent(wxCommandEvent &);

    void OnButtonUpEvent(wxCommandEvent &);

    void OnButtonEvent(wxCommandEvent &);

    void CreateMenu();

    void OnCloseWindow(wxCloseEvent &);

    void CreateButton(int, pchar title, const wxPoint &, int width);
};
