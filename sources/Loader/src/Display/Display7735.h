// 2024/03/01 22:45:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Display
{
    static const int WIDTH = 160;
    static const int HEIGHT = 80;

    static const int NUMBER_PARTS_HEIGHT = 8;          // Ќа столько частей разбит дислей. —только раз будем отрисовывать изображение,
                                                // чтобы получить полную картинку

    void Init();

    void Update();

    uint TimeEnabled();

    bool IsOldType();

    namespace Buffer
    {
        static const uint SIZE = WIDTH * HEIGHT / NUMBER_PARTS_HEIGHT;

        // ¬озвращает указатель на строку
        uint8 *GetLine(int y);
    }
}


struct Pixel
{
    void Set(int x, int y) const;
};


struct HLine
{
    HLine(int w) : width(w) { }
    void Draw(int x, int y) const;
private:
    const int width;
};


struct VLine
{
    VLine(int h) : height(h) { }
    void Draw(int x, int y) const;
private:
    const int height;
};


struct Rect
{
    Rect(int w, int h) : width(w), height(h) { }
    void Draw(int x, int y) const;
    void Fill(int x, int y) const;
private:
    const int width;
    const int height;
};
