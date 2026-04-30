#pragma once

#include <iostream>

namespace Verify
{
    // true jesli sortowane poprawnie
    template <typename T>
    bool isSorted(const T *data, int size)
    {
        for (int i = 0; i < size - 1; ++i)
        {
            if (data[i] > data[i + 1])
                return false;
        }
        return true;
    }

    // wypisuje czy sortowanie poprawne
    template <typename T>
    void check(const T *data, int size)             //wrapper dla isSorted
    {
        if (isSorted(data, size))
            std::cout << "OK: posortowane poprawnie.\n";
        else
            std::cout << "BŁaD: sortowanie niepoprawne!\n";
    }
}
