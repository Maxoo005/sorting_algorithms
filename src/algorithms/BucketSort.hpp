#pragma once

#include <algorithm>
#include <cmath>

// Bucket sort
// Złożoność czasowa: O(n + k) średnio, gdzie k = liczba kubełków (sqrt(n))
// Złożoność pamięciowa: O(n + k)
// Ręczne zarządzanie pamięcią – bez std::vector

namespace BucketSort
{
    namespace detail
    {
        // Prosta tablica dynamiczna do przechowywania elementów jednego kubełka
        template <typename T>
        struct Bucket
        {
            T   *data;
            int  size;
            int  cap;

            Bucket() : data(nullptr), size(0), cap(0) {}

            ~Bucket() { delete[] data; }

            // kopiowanie zablokowane – surowy wskaźnik, double-free
            Bucket(const Bucket &) = delete;
            Bucket &operator=(const Bucket &) = delete;

            void push(const T &val)
            {
                if (size == cap)
                {
                    int newCap = (cap == 0) ? 4 : cap * 2;
                    T  *tmp    = new T[newCap];
                    for (int i = 0; i < size; ++i) tmp[i] = data[i];
                    delete[] data;
                    data = tmp;
                    cap  = newCap;
                }
                data[size++] = val;
            }

            // insertion sort – kubełki są małe, wbudowane sortowanie wystarczy
            void insertionSort()
            {
                for (int i = 1; i < size; ++i)
                {
                    T key = data[i];
                    int j = i - 1;
                    while (j >= 0 && data[j] > key)
                    {
                        data[j + 1] = data[j];
                        --j;
                    }
                    data[j + 1] = key;
                }
            }
        };
    }

    template <typename Container>
    void sort(Container &c)
    {
        const int n = c.size();
        if (n <= 1) return;

        using T = typename Container::value_type;

        // Znajdź min i max
        T minVal = c.get(0);
        T maxVal = c.get(0);
        for (int i = 1; i < n; ++i)
        {
            T v = c.get(i);
            if (v < minVal) minVal = v;
            if (v > maxVal) maxVal = v;
        }

        if (minVal == maxVal) return;   // wszystkie elementy identyczne – nic do roboty

        // Liczba kubełków: sqrt(n), min 2
        const int k = (std::sqrt((double)n) < 2.0) ? 2 : (int)std::sqrt((double)n);

        // Alokuj kubełki ręcznie
        detail::Bucket<T> *buckets = new detail::Bucket<T>[k];

        // Rozdziel elementy – long long chroni przed overflow (INT_MIN..INT_MAX)
        const long long range = (long long)maxVal - (long long)minVal;
        for (int i = 0; i < n; ++i)
        {
            T v = c.get(i);
            long long idx = (long long)(v - minVal) * (k - 1) / range;
            buckets[(int)idx].push(v);
        }

        // Posortuj każdy kubełek i przepisz z powrotem
        int pos = 0;
        for (int b = 0; b < k; ++b)
        {
            buckets[b].insertionSort();
            for (int i = 0; i < buckets[b].size; ++i)
                c.set(pos++, buckets[b].data[i]);
        }

        delete[] buckets;
    }
}
