// 2026/02/10 22:46:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


class MonochromeBitmap {
private:
#pragma pack(push, 1)  // Выравнивание 1 байт для структур BMP
    struct BITMAPFILEHEADER
    {
        uint16 bfType;
        uint   bfSize;
        uint16 bfReserved1;
        uint16 bfReserved2;
        uint   bfOffBits;
    };

    struct BITMAPINFOHEADER
    {
        uint   biSize;
        int    biWidth;
        int    biHeight;
        uint16 biPlanes;
        uint16 biBitCount;
        uint   biCompression;
        uint   biSizeImage;
        int    biXPelsPerMeter;
        int    biYPelsPerMeter;
        uint   biClrUsed;
        uint   biClrImportant;
    };
#pragma pack(pop)

    int width;
    int height;
    int row_bytes;                  // количество байт в строке с учетом выравнивания
    const uint8 *data_begin = 0;    // непосредственное начало пикселей
    uint pixel_data_size = 0;       // размер данных пикселей в байтах

public:

    // Конструктор из массива данных (уже прочитанного файла)
    MonochromeBitmap(const uint8 *bmp_data)
    {
        const BITMAPFILEHEADER *file_header =
            reinterpret_cast<const BITMAPFILEHEADER *>(bmp_data);
        const BITMAPINFOHEADER *info_header =
            reinterpret_cast<const BITMAPINFOHEADER *>(bmp_data + sizeof(BITMAPFILEHEADER));

        width = info_header->biWidth;
        height = info_header->biHeight;

        // Вычисляем количество байт в строке с выравниванием до 4 байт
        row_bytes = ((width + 31) / 32) * 4;

        // Копируем данные пикселей (после палитры)
        uint data_offset = file_header->bfOffBits;
        pixel_data_size = (uint)row_bytes * height;

        data_begin = bmp_data + data_offset;
    }

    // Получение ширины изображения
    int GetWidth() const
    {
        return width;
    }

    // Получение высоты изображения
    int GetHeight() const
    {
        return height;
    }

    // Получение цвета точки
    // true = белый (бит установлен), false = черный (бит сброшен)
    // или наоборот, в зависимости от палитры
    bool GetPoint(int x, int y) const
    {
        // Проверка границ
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            return false;
        }

        // BMP может храниться снизу-вверх (положительная высота)
        // или сверху-вниз (отрицательная высота)
        int actual_y;
        if (height > 0)
        {
            // Снизу-вверх
            actual_y = height - 1 - y;
        }
        else
        {
            // Сверху-вниз
            actual_y = y;
        }

        // Вычисляем позицию байта
        int byte_index = x / 8;
        int bit_position = 7 - (x % 8);  // В BMP старший бит первый

        // Получаем байт
        const uint8 *row_start = data_begin + (actual_y * row_bytes);
        uint8 target_byte = row_start[byte_index];

        // Проверяем бит
        return (target_byte >> bit_position) & 0x01;
    }

    void Draw(int x, int y, bool inverse);
};