#pragma once

#include <algorithm>

// Tablica dynamiczna z interfejsem dla alg sortowania
// Dostęp losowy O(1), swap O(1)
template <typename T>
class SortArray
{
public:
    using value_type = T;

    // konstruktor z pojemnością – alokuje tablicę, wartości niezainicjowane
    // używamy gdy znamy rozmiar z góry i sami zapełnimy tablicę
    explicit SortArray(int capacity)
        : m_data(new T[capacity]), m_size(capacity) {}

    // konstruktor kopiujący dane ze zwykłej tablicy (np. wczytanej z pliku)
    SortArray(const T *src, int size)
        : m_data(new T[size]), m_size(size)
    {
        for (int i = 0; i < size; ++i)
            m_data[i] = src[i];
    }

    ~SortArray() { delete[] m_data; }

    // kopiowanie zablokowane – ręczne zarządzanie pamięcią
    // dwa obiekty wskazujące na ten sam m_data = double free = crash
    SortArray(const SortArray &) = delete;
    SortArray &operator=(const SortArray &) = delete;

    // interfejs wspólny z listami – algorytmy sortowania używają właśnie get/set/swap
    int      size()                    const { return m_size; }
    T       &get(int i)                      { return m_data[i]; }   // O(1)
    const T &get(int i)                const { return m_data[i]; }
    void     set(int i, const T &val)        { m_data[i] = val; }    // O(1)
    T       &operator[](int i)               { return m_data[i]; }
    const T &operator[](int i)         const { return m_data[i]; }
    void     swap(int i, int j)              { std::swap(m_data[i], m_data[j]); } // O(1)

private:
    T   *m_data;
    int  m_size;
};
