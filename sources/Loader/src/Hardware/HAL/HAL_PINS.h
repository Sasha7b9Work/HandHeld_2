// 2024/03/01 22:10:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct ModePin
{
    enum E
    {
        Output,
        Input,
        Count
    };
};


struct Pin
{
    Pin(uint _port, uint _pin, ModePin::E _mode) : port(_port), pin(_pin), mode(_mode) { }
    void Init();
protected:
    uint       port;
    uint       pin;
    ModePin::E mode;
};


struct PinIn : public Pin
{
    PinIn(uint _port, uint _pin) : Pin(_port, _pin, ModePin::Input) {}
    void Init();
    bool IsHi() const;
    bool IsLow() const { return !IsHi(); }
};


struct PinOut : public Pin
{
    PinOut(uint _port, uint _pin) : Pin(_port, _pin, ModePin::Output) { }
    void Init();
    void ToLow() const;
    void ToHi() const;
    void Set(bool enable) const { enable ? ToHi() : ToLow(); }
    bool IsHi() const;
};


extern PinOut pinSPI1_NSS;
extern PinOut pinSPI1_CLK;
extern PinOut pinSPI1_MOSI;
extern PinIn  pinSPI_MISO;

extern PinIn btnUp;
extern PinIn btnDown;
extern PinIn btnLeft;
extern PinIn btnRight;
