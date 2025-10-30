// 2024/03/07 20:23:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Math
{
    template<class T>
    void CircleIncrease(T *value, T min, T max)
    {
        (*value)++;

        if (*value < min)
        {
            *value = max;
        }
        else if (*value > max)
        {
            *value = min;
        }
    }

    template<class T>
    void CircleDecrease(T *value, T min, T max)
    {
        (*value)--;

        if (*value < min)
        {
            *value = max;
        }
        else if (*value > max)
        {
            *value = min;
        }
    }

    template<class T>
    void CircleChange(T *value, T min, T max, T delta)
    {
        *value = *value + delta;

        if (*value < min)
        {
            *value = max;
        }
        else if (*value > max)
        {
            *value = min;
        }
    }

    inline uint Hash(uint hash, char byte)
    {
        return (uint8)byte + (hash << 6) + (hash << 16) - hash;
    }

    inline uint CalculateCRC(const void *address, int size)
    {
        uint crc = 0;

        const uint8 *data = (const uint8 *)address;

        for (int i = 0; i < size; i++)
        {
            crc = Math::Hash(crc, (char)*data++);
        }

        return crc;
    }

    uint CalculateCRC32(const void *data, int size);
}
