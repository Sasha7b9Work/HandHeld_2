// 2024/03/02 13:56:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Keyboard/Keyboard.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Display/Text.h"


struct ItemType
{
    enum E
    {
        Page,
        Button,
        CustomButton,
        DateTime,
        Choice,
        Count
    };
};


struct Item;
struct Page;
struct DateTime;
struct Choice;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Item
struct DataItem
{
    ItemType::E type;
    const Item *keeper;     // ������ �����
    pchar       title;
    int8 *const opened;     // ���� �� ����� ����, �� ���� �������

};


struct Item
{
    const DataItem *data;
    const void     *item;     // ��������� �� ��������� ���� ItemType
    void Draw() const;
    bool IsPage() const { return data->type == ItemType::Page; }
    bool IsDateTime() const { return data->type == ItemType::DateTime; }
    bool IsChoice() const { return data->type == ItemType::Choice; }
    const Page *GetPage() const { if (IsPage()) { return (const Page *)item; } return nullptr; }
    const DateTime *GetDateTime() const { if (IsDateTime()) { return (const DateTime *)item; } return nullptr; }
    const Choice *GetChoice() const { if (IsChoice()) { return (const Choice *)item; } return nullptr; }
    void ApplyAction(const Action &) const;
    bool IsOpened() const;
    Text<> Title() const;
    void Open() const;
    void Close() const;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Page
struct DataPage
{
    const Item     *item;           // � ���� ����� �������� ������ ���������
    const Item   **items;
    int8 * const   current_item;    // ������� ������� ��������
    void (*func_draw_closed)();
    void (*func_draw_opened)();
    bool (*func_action_key)(const Action &);
};


struct Page
{
    const DataPage *data;
    void Draw() const;
    const Item *CurrentItem() const;
    const DataItem *GetDataItem() const;
    void ApplyAction(const Action &) const;
    // ������� ������� ��������� �������
    void NextCurrentItem() const;
    // ������� ������� ���������� �������
    void PrevCurrentItem() const;
    int NumItems() const;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Date
struct DataDateTime
{
    const Item *item;
    int8 *const field;          // �������� ���� 0 - ����, 1 - �����, 2 - ���
    RTCDateTime * const date_time;
    const bool is_time;
    const bool is_alarm;        // ���� true - �� ��������� ����������
};


struct DateTime
{
    const DataDateTime *data;
    void Draw() const;
    void ApplyAction(const Action &) const;
private:
    void ChangeValueInCurrentField(int delta) const;
    void DrawField(int x, int y, Text<> &, bool selected) const;
    int NumFields() const { return data->is_time ? 2 : 3; }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Choice
struct DataChoice
{
    const Item *item;
    void (*func_on_change)();
    uint8 *const value;
    const pchar * const names;
    const Color::E * const colors;
};


struct Choice
{
    const DataChoice *data;
    void Draw() const;
    void ApplyAction(const Action &) const;
    int NumChoices() const;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Button
struct DataButton
{

};


struct Button
{
    DataButton *data;
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// CustomButton
struct DataCustomButton
{
    int x;
    int y;
    int width;
    int height;
    void (*funcPress)();
    void (*funcDraw)();
};


struct CustomButton
{
    DataButton *data;
};
