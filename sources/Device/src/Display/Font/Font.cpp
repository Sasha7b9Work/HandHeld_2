// 2023/04/17 14:03:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Font/Font.h"
#include "Display/Display.h"
#include "Display/Font/fontGOSTAU16BOLD.inc"
#include "Display/Font/fontGostB48B.inc"
#include "Display/Text.h"
#include "Display/Primitives.h"


#ifdef MODEL7789

struct NativeSymbol
{
    uint8 width;        // Ширина символа в пикселях
    uint8 height;       // Высота символа в пикселях
    uint8 firstRow;     // Номер первой непустой строки. Именно её первый байт хранится в data

    // Возвращает количество байт в строке
    int BytesInRow()
    {
        int result = width / 8;

        if (width % 8)
        {
            result++;
        }

        return result;
    }
    // Возвращает указатель на первый байт строки
    uint8 *GetRow(int row)
    {
        if (row > height - 1)
        {
            return nullptr;
        }

        if (row < firstRow)
        {
            return nullptr;
        }

        return Data() + (row - firstRow) * BytesInRow();
    }
    // Возвращает указатель на первый байт данных
    uint8 *Data();

    bool BitIsExist(int r, int b)
    {
        uint8 *row = GetRow(r);

        if (row == nullptr)
        {
            return false;
        }

        while (b > 7)       // Перемещаеммся к байту, содержащему наш бит
        {
            row++;
            b -= 8;
        }

        return ((*row) & (1 << (7 - b))) != 0;
    }
};


// Структрура заголовка
struct HeaderFont
{
    uint16       offsets[256];  // Смещения 256 символов таблицы. 0 означает, что символ отсутствует
    NativeSymbol symbol;        // Первый символ в таблице его смещение 256

    // Возвращает указатель на символ, если он присутствует в таблице и nullptr в обратном случае
    NativeSymbol *GetSymbol(uint8 num)
    {
        HeaderFont *header = HeaderFont::Sefl();

        if (header->offsets[num] == 0)
        {
            return nullptr;
        }

        uint8 *offset = reinterpret_cast<uint8 *>(header) + header->offsets[num];

        return reinterpret_cast<NativeSymbol *>(offset);
    }

    static HeaderFont *Sefl();
};

static const unsigned char *font = nullptr;

HeaderFont *HeaderFont::Sefl()
{
    return reinterpret_cast<HeaderFont *>(const_cast<uint8 *>(font)); //-V2567
}

uint8 *NativeSymbol::Data()
{
    return reinterpret_cast<uint8 *>(this) + sizeof(*this);
}


#endif


struct DSymbol
{
    uchar width;
    uchar bytes[8];
};

struct DTypeFont
{
    enum E
    {
        _5,
        _8,
        Count,
        None
    };
};

struct DFont
{
    int _height;
    DSymbol symbols[256];
};


#include "Display/Font/font5.inc"
#include "Display/Font/font8.inc"


static const DFont *dfont = &font8;


namespace Font
{
#ifdef MODEL7735
    TypeFont::E type = TypeFont::_7;
#endif

#ifdef MODEL7789
    TypeFont::E type = TypeFont::GOSTB28B;
#endif

    static int size = 1;
}


void Font::SetSize(int _size)
{
    (void)_size;

#ifdef MODEL7735
    size = _size;
#endif
}


int Font::GetSize()
{
    return size;
}


void Font::SetMainType()
{
#ifdef MODEL7735
    SetType(TypeFont::_7);
#endif

#ifdef MODEL7789
    SetType(TypeFont::GOSTB28B);
#endif
}


void Font::SetSmallType()
{
#ifdef MODEL7735
    Font::SetType(TypeFont::_5);
#endif

#ifdef MODEL7789
    Font::SetType(TypeFont::GOSTAU16BOLD);
#endif
}


void Font::SetType(TypeFont::E _type)
{
    type = _type;

#ifdef MODEL7735
    if (_type == TypeFont::_5)
    {
        dfont = &font5;
    }
    else if (_type == TypeFont::_7)
    {
        dfont = &font8;
    }
#endif

#ifdef MODEL7789
    if (_type == TypeFont::GOSTAU16BOLD)
    {
        font = fontGOSTAU16BOLD;
    }
    else if (_type == TypeFont::GOSTB28B)
    {
        font = fontGostB48B;
    }
#endif
}


int Font::GetBit(uint8 symbol, int row, int bit)
{
    return dfont->symbols[symbol].bytes[row] & (1 << (7 - bit));
}

int Font::GetHeight()
{
#ifdef MODEL7735
    switch (type)
    {
    case TypeFont::_5:      return 5;
    case TypeFont::_7:      return 7;
    case TypeFont::_Count:
        break;
    }
#endif

#ifdef MODEL7735
    if (type == TypeFont::_5)
    {
        return 5;
    }
    else if (type == TypeFont::_7)
    {
        return 7;
    }
#endif

#ifdef MODEL7789
    if (type == TypeFont::GOSTAU16BOLD || type == TypeFont::GOSTB28B)
    {
        uint8 result = 0;

        for (int i = 0; i < 256; i++)
        {
            NativeSymbol *symbol = HeaderFont::Sefl()->GetSymbol(static_cast<uint8>(i));

            if (symbol && symbol->height > result)
            {
                result = symbol->height;
            }
        }

        return result;
    }
#endif

    return 10;
}


int Font::GetWidth(uint8 symbol)
{
#ifdef MODEL7735
    if (type == TypeFont::_5 || type == TypeFont::_7)
    {
        return dfont->symbols[symbol].width;
    }
#endif

#ifdef MODEL7789
    if (type == TypeFont::GOSTAU16BOLD || type == TypeFont::GOSTB28B)
    {
        if (symbol == 0x20)
        {
            if (font == fontGOSTAU16BOLD)
            {
                return 5;
            }
        }

        if (symbol == '1')
        {
            return GetWidth('0');
        }

        NativeSymbol *sym = HeaderFont::Sefl()->GetSymbol(symbol);

        return sym ? (int)sym->width : 0;
    }
#endif

    return 10;
}


int Char::Write(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    return Write(x, y);
}


int Char::Write(int x, int y) const
{
#ifdef MODEL7735
    if (Font::type == TypeFont::_5 || Font::type == TypeFont::_7)
    {
        int height = Font::GetHeight();
        int width = Font::GetWidth(symbol);

        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                if (Font::GetBit(symbol, row, col))
                {
                    Rect(Font::size, Font::size).Fill(x + col * Font::size, y + row * Font::size);
                }
            }
        }

        return x + width * Font::size;
    }
#endif

#ifdef MODEL7789
    if (Font::type == TypeFont::GOSTAU16BOLD || Font::type == TypeFont::GOSTB28B)
    {
        int height = Font::GetHeight();
        int width = Font::GetWidth(symbol);

        if (Font::size == 1)
        {
            for (int row = 0; row < height; row++)
            {
                if (Font::RowNotEmpty(symbol, row))
                {
                    for (int col = 0; col < width; col++)
                    {
                        if (Font::BitIsExist(symbol, row, col))
                        {
                            Pixel().Set(x + col, y + row);
                        }
                    }
                }
            }
        }
        else
        {
            for (int row = 0; row < height; row++)
            {
                if (Font::RowNotEmpty(symbol, row))
                {
                    for (int col = 0; col < width; col++)
                    {
                        if (Font::BitIsExist(symbol, row, col))
                        {
                            Rect(Font::size, Font::size).Fill(x + col * Font::size, y + row * Font::size);
                        }
                    }
                }
            }
        }

        return x + width * Font::size;
    }
#endif

    return x;
}


#ifdef MODEL7789

bool Font::RowNotEmpty(uint8 s, int r)
{
    HeaderFont *header = HeaderFont::Sefl();

    NativeSymbol *symbol = header->GetSymbol(s);

    if (symbol)
    {
        if (r < symbol->firstRow)
        {
            return false;
        }

        uint8 *row = symbol->GetRow(r);

        if (row)
        {
            for (int i = 0; i < symbol->BytesInRow(); i++)
            {
                if (row[i] != 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}


bool Font::BitIsExist(uint8 s, int row, int bit)
{
    NativeSymbol *symbol = HeaderFont::Sefl()->GetSymbol(s);

    return symbol ? symbol->BitIsExist(row, bit) : false;
}

#endif
