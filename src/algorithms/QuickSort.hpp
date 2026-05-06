#pragma once

#include "Parameters.h"
#include <random>

// Quick sort
// Złożoność czasowa: O(n log n) średnio, O(n²) pesymistycznie
// Złożoność pamięciowa: O(log n) – stos rekurencji
// Strategia wyboru pivota kontrolowana przez Parameters::pivot

namespace QuickSort
{
    namespace detail
    {
        // generator dla pivot losowego – static gwarantuje jedną inicjalizację
        inline std::mt19937 &rng()
        {
            static std::mt19937 gen(std::random_device{}());
            return gen;
        }

        template <typename Container>
        int choosePivotIdx(Container &c, int lo, int hi)
        {
            (void)c; // c nieużywane przy wyborze indeksu
            switch (Parameters::pivot)
            {
                case Parameters::Pivots::left:   return lo;
                case Parameters::Pivots::right:  return hi;
                case Parameters::Pivots::middle: return lo + (hi - lo) / 2;
                default: // random (undefined też trafia tutaj)
                {
                    std::uniform_int_distribution<int> dist(lo, hi);
                    return dist(rng());
                }
            }
        }

        // Schemat Lomuto – pivot przeniesiony na koniec, potem partycja
        template <typename Container>
        int partition(Container &c, int lo, int hi)
        {
            const int pivotIdx = choosePivotIdx(c, lo, hi);
            const auto pivotVal = c.get(pivotIdx);
            c.swap(pivotIdx, hi);   // przesuń pivot na koniec

            int store = lo;
            for (int i = lo; i < hi; ++i)
            {
                if (c.get(i) <= pivotVal)
                {
                    c.swap(i, store);
                    ++store;
                }
            }
            c.swap(store, hi);      // pivot na właściwe miejsce
            return store;
        }

        template <typename Container>
        void sort(Container &c, int lo, int hi)
        {
            if (lo >= hi) return;
            const int p = partition(c, lo, hi);
            sort(c, lo, p - 1);
            sort(c, p + 1, hi);
        }
    }

    template <typename Container>
    void sort(Container &c)
    {
        if (c.size() <= 1) return;
        detail::sort(c, 0, c.size() - 1);
    }
}
