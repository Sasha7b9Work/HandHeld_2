#pragma once



static const Note notes10[] =
{
    { 500,  4, 9 },
    { 5000, 4, 8 },
    { 0,    0, 0}
};


static const Sound sound10 =
{
    notes10, nullptr
};


/*
// �����
#define M_2_3     155
#define M_4_5     185
#define M_5       196
#define M_6_7     233

// ������
#define First_2   294
#define First_2_3 311
#define First_3   330
#define First_4   349
#define First_4_5 370


//
// https://dominantamusic.ru/blog/pesni-na-sintezatore-po-tsifram/
// �������� ����
//

static const Note notes10[] =
{
    { 196,  10, 9 },  // � 5
    { 196,  10, 8 },  // � 5
    { 196,  10, 9 },  // � 5
    { 155,  10, 8 },  // � 2/3
    { 233,  10, 9 },  // � 6/7
    { 196,  10, 9 },  // � 5
    { 155,  10, 9 },  // � 2/3
    { 233,  10, 9 },  // � 6/7
    { 196,  10, 9 },  // � 5

    { First_2,   10, 9 },
    { First_2,   10, 9 },
    { First_2,   10, 9 },
    { First_4_5, 10, 9 },
    { M_6_7,     10, 9 },
    { M_4_5,     10, 9 },
    { M_2_3,     10, 9 },
    { M_6_7,     10, 9 },
    { M_5,       10, 9 },

    { 0,    0, 0}
};


static const Sound sound10 =
{
    notes10, nullptr
};
*/
