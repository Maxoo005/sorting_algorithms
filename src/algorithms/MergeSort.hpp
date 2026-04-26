#pragma once

#include <vector>

// Merge sort – działa na każdej strukturze z interfejsem get(i)/set(i)/size()
// Złożoność czasowa: O(n log n)
// Złożoność pamięciowa: O(n) – bufor pomocniczy
// Uwaga: dla list get(i) to O(n), więc całość staje się O(n² log n) –
//        widoczne w benchmarkach jako różnica między tablicą a listą.

namespace MergeSort
{
    namespace detail
    {
        template <typename Container>
        void merge(Container &c, int left, int mid, int right,
                   std::vector<typename Container::value_type> &tmp)
        {
            // Skopiuj zakres do bufora
            for (int i = left; i <= right; ++i)
                tmp[i] = c.get(i);

            int i = left;
            int j = mid + 1;
            int k = left;

            while (i <= mid && j <= right)
            {
                if (tmp[i] <= tmp[j])
                    c.set(k++, tmp[i++]);
                else
                    c.set(k++, tmp[j++]);
            }
            while (i <= mid)  c.set(k++, tmp[i++]);
            while (j <= right) c.set(k++, tmp[j++]);
        }

        template <typename Container>
        void sort(Container &c, int left, int right,
                  std::vector<typename Container::value_type> &tmp)
        {
            if (left >= right) return;
            int mid = left + (right - left) / 2;
            sort(c, left,    mid,   tmp);
            sort(c, mid + 1, right, tmp);
            merge(c, left, mid, right, tmp);
        }
    }

    template <typename Container>
    void sort(Container &c)
    {
        if (c.size() <= 1) return;
        std::vector<typename Container::value_type> tmp(c.size());
        detail::sort(c, 0, c.size() - 1, tmp);
    }
}
