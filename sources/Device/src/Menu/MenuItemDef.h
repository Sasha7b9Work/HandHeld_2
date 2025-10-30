// 2024/03/06 21:37:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/PCF8563/PCF8563.h"


#define DEF_TIME(name, keeper, title, is_alarm)                                                                             \
static int8 opened##name = 0;                                                                                               \
static int8 field##name = 0;                                                                                                \
static const DataItem name##data = { ItemType::DateTime, keeper, title, &opened##name };                                    \
extern const Item name;                                                                                                     \
static RTCDateTime name##date_time;                                                                                         \
static const DataDateTime name##datatime = { &name, &field##name, &name##date_time, true, is_alarm };                       \
static const DateTime name##time = { &name##datatime };                                                                     \
const Item name = {&name##data, &name##time }

#define DEF_DATE(name, keeper, title, is_alarm)                                                                             \
static int8 opened##name = 0;                                                                                               \
static int8 field##name = 0;                                                                                                \
static const DataItem name##data = { ItemType::DateTime, keeper, title, &opened##name };                                    \
extern const Item name;                                                                                                     \
static RTCDateTime name##date_time;                                                                                         \
static const DataDateTime name##datatime = { &name, &field##name, &name##date_time, false, is_alarm };                      \
static const DateTime name##time = { &name##datatime };                                                                     \
const Item name = {&name##data, &name##time }

#define DEF_PAGE_0(name, keeper, title, func_draw_closed, func_draw_opened, func_key)                                       \
static const Item *items##name[] = { nullptr };                                                                             \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_1(name, keeper, title, item0, func_draw_closed, func_draw_opened, func_key)                                \
static const Item *items##name[] = { item0, nullptr };                                                                      \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_2(name, keeper, title, item0, item1, func_draw_closed, func_draw_opened, func_key)                         \
static const Item *items##name[] = { item0, item1, nullptr };                                                               \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_3(name, keeper, title, item0, item1, item2, func_draw_closed, func_draw_opened, func_key)                  \
static const Item *items##name[] = { item0, item1, item2, nullptr };                                                        \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_4(name, keeper, title, item0, item1, item2, item3, func_draw_closed, func_draw_opened, func_key)           \
static const Item *items##name[] = { item0, item1, item2, item3, nullptr };                                                 \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_5(name, keeper, title, item0, item1, item2, item3, item4, func_draw_closed, func_draw_opened,              \
    func_key)                                                                                                               \
static const Item *items##name[] = { item0, item1, item2, item3, item4, nullptr };                                          \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_6(name, keeper, title, item0, item1, item2, item3, item4, item5, func_draw_closed, func_draw_opened,       \
    func_key)                                                                                                               \
static const Item *items##name[] = { item0, item1, item2, item3, item4, item5, nullptr };                                   \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_7(name, keeper, title, item0, item1, item2, item3, item4, item5, item6,                                    \
    func_draw_closed, func_draw_opened, func_key)                                                                           \
static const Item *items##name[] = { item0, item1, item2, item3, item4, item5, item6, nullptr };                            \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_8(name, keeper, title, item0, item1, item2, item3, item4, item5, item6, item7,                             \
    func_draw_closed, func_draw_opened, func_key)                                                                           \
static const Item *items##name[] = { item0, item1, item2, item3, item4, item5, item6, item7, nullptr };                     \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_PAGE_9(name, keeper, title, item0, item1, item2, item3, item4, item5, item6, item7, item8,                      \
    func_draw_closed, func_draw_opened, func_key)                                                                           \
static const Item *items##name[] = { item0, item1, item2, item3, item4, item5, item6, item7, item8, nullptr };              \
static int8 current_item##name = 0;                                                                                         \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Page, keeper, title, &opened##name};                                               \
extern const Item name;                                                                                                     \
static const DataPage dp##name{&name, items##name, &current_item##name, func_draw_closed, func_draw_opened, func_key};      \
static const Page page##name{&dp##name};                                                                                    \
const Item name{&di##name, &page##name}

#define DEF_CHOICE_2(name, keeper, title, value, name0, name1, func_on_change)                                              \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Choice, keeper, title, &opened##name};                                             \
extern const Item name;                                                                                                     \
static const pchar names##name[] = {name0, name1, nullptr};                                                                 \
static const DataChoice dc##name{&name, func_on_change, value, &names##name[0], nullptr };                                  \
static const Choice choice##name{&dc##name};                                                                                \
const Item name{&di##name, &choice##name}

#define DEF_CHOICE_3(name, keeper, title, value, name0, name1, name2, func_on_change)                                       \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Choice, keeper, title, &opened##name};                                             \
extern const Item name;                                                                                                     \
static const pchar names##name[] = {name0, name1, name2, nullptr };                                                         \
static const DataChoice dc##name{&name, func_on_change, value, &names##name[0], nullptr };                                  \
static const Choice choice##name{&dc##name};                                                                                \
const Item name{&di##name, &choice##name}

#define DEF_CHOICE_7(name, keeper, title, value, name0, name1, name2, name3, name4, name5, name6, func_on_change)           \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Choice, keeper, title, &opened##name};                                             \
extern const Item name;                                                                                                     \
static const pchar names##name[] = {name0, name1, name2, name3, name4, name5, name6, nullptr };                             \
static const DataChoice dc##name{&name, func_on_change, value, &names##name[0], nullptr };                                  \
static const Choice choice##name{&dc##name};                                                                                \
const Item name{&di##name, &choice##name}

#define DEF_CHOICE_8(name, keeper, title, value, name0, name1, name2, name3, name4, name5, name6, name7, func_on_change)    \
static int8 opened##name = 0;                                                                                               \
static const DataItem di##name{ItemType::Choice, keeper, title, &opened##name};                                             \
extern const Item name;                                                                                                     \
static const pchar names##name[] = {name0, name1, name2, name3, name4, name5, name6, name7, nullptr };                      \
static const DataChoice dc##name{&name, func_on_change, value, &names##name[0], nullptr };                                  \
static const Choice choice##name{&dc##name};                                                                                \
const Item name{&di##name, &choice##name}

#define DEF_CHOICE_7_COLORS(name, keeper, title, value, name0, name1, name2, name3, name4, name5, name6, func_on_change)         \
static int8 opened##name = 0;                                                                                                    \
static const DataItem di##name{ItemType::Choice, keeper, title, &opened##name};                                                  \
extern const Item name;                                                                                                          \
static const pchar names##name[] = {name0, name1, name2, name3, name4, name5, name6, nullptr };                                  \
static const Color::E colors##name[] = { Color::White, Color::Red, Color::Green, Color::Blue, Color::Yellow, Color::Cyan,        \
    Color::Magenta };                                                                                                            \
static const DataChoice dc##name{&name, func_on_change, value, &names##name[0], &colors##name[0] };                              \
static const Choice choice##name{&dc##name};                                                                                     \
const Item name{&di##name, &choice##name}

#define DEF_CHOICE_8_COLORS(name, keeper, title, value, name0, name1, name2, name3, name4, name5, name6, name7, func_on_change)  \
static int8 opened##name = 0;                                                                                                    \
static const DataItem di##name{ItemType::Choice, keeper, title, &opened##name};                                                  \
extern const Item name;                                                                                                          \
static const pchar names##name[] = {name0, name1, name2, name3, name4, name5, name6, name7, nullptr };                           \
static const Color::E colors##name[] = { Color::White, Color::Red, Color::Green, Color::Blue, Color::Gray, Color::Yellow,        \
    Color::Orange, Color::Purple };                                                                                              \
static const DataChoice dc##name{&name, func_on_change, value, &names##name[0], &colors##name[0] };                              \
static const Choice choice##name{&dc##name};                                                                                     \
const Item name{&di##name, &choice##name}

#define DEF_CHOICE_10(name, keeper, title, value, name0, name1, name2, name3, name4, name5, name6, name7, name8, name9, func_on_change)  \
static int8 opened##name = 0;                                                                                                            \
static const DataItem di##name{ItemType::Choice, keeper, title, &opened##name};                                                          \
extern const Item name;                                                                                                                  \
static const pchar names##name[] = {name0, name1, name2, name3, name4, name5, name6, name7, name8, name9, nullptr };                     \
static const DataChoice dc##name{&name, func_on_change, value, &names##name[0], nullptr };                                               \
static const Choice choice##name{&dc##name};                                                                                             \
const Item name{&di##name, &choice##name}



#define DEF_CHOICE_MELODY(set)                                              \
DEF_CHOICE_10(choiceMelody, self, "Ã≈ÀŒƒ»ﬂ", (uint8 *const)&set,            \
    Melody::Name(Melody::_1),                                               \
    Melody::Name(Melody::_2),                                               \
    Melody::Name(Melody::_3),                                               \
    Melody::Name(Melody::_4),                                               \
    Melody::Name(Melody::_5),                                               \
    Melody::Name(Melody::_6),                                               \
    Melody::Name(Melody::_7),                                               \
    Melody::Name(Melody::_8),                                               \
    Melody::Name(Melody::_9),                                               \
    Melody::Name(Melody::_10),                                              \
    nullptr                                                                 \
);

#define DEF_CHOICE_MODE_INDICATION(set, func)                               \
DEF_CHOICE_7(choiceModeIndication, self, "ŒœŒ¬≈Ÿ≈Õ»≈", (uint8 *const)&set,  \
    "«¬” ",                                                                 \
    "—¬≈“",                                                                 \
    "¬»¡–Œ",                                                                \
    "«¬” +—¬≈“",                                                            \
    "«¬” +¬»¡–Œ",                                                           \
    "—¬≈“+¬»¡–Œ",                                                           \
    "¬—≈",                                                                  \
    func                                                                    \
);

#define DEF_CHOICE_VOLUME(set)                                              \
DEF_CHOICE_3(choiceVolume, self, "√–ŒÃ Œ—“‹", (uint8 *const)&set,           \
    "Õ»« ¿ﬂ",                                                               \
    "—–≈ƒÕﬂﬂ",                                                              \
    "¬€—Œ ¿ﬂ",                                                              \
    nullptr                                                                 \
);

#define DEF_CHOICE_COLOR(set)                                               \
DEF_CHOICE_7_COLORS(choiceColor, self, "÷¬≈“", (uint8 *const)&set,          \
    Color::Name(Color::White),                                              \
    Color::Name(Color::Red),                                                \
    Color::Name(Color::Green),                                              \
    Color::Name(Color::Blue),                                               \
    Color::Name(Color::Yellow),                                             \
    Color::Name(Color::Cyan),                                               \
    Color::Name(Color::Magenta),                                            \
    nullptr                                                                 \
);

#define DEF_SOURCE_CHOICES(source, func)                                    \
DEF_CHOICE_MODE_INDICATION(gset.sources[source].mode_indication, func);     \
DEF_CHOICE_MELODY(gset.sources[source].melody);                             \
DEF_CHOICE_VOLUME(gset.sources[source].volume);                             \
DEF_CHOICE_COLOR(gset.sources[source].color);

#define DEF_PAGE_SOURCE(name, source, func)             \
DEF_SOURCE_CHOICES(source, func);                       \
DEF_PAGE_4(name, PageMain::self, Source::Name(source),  \
    &choiceModeIndication,                              \
    &choiceMelody,                                      \
    &choiceVolume,                                      \
    &choiceColor,                                       \
    nullptr,                                            \
    nullptr,                                            \
    nullptr                                             \
);                                                      \
const Item *const self = &name;
