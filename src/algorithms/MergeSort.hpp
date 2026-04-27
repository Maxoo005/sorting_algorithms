#pragma once

// Merge sort 
// Złożoność czasowa: O(n log n)
// Złożoność pamięciowa: O(n) – jeden bufor alokowany raz na całe sortowanie

namespace MergeSort
{
    namespace detail
    {
        template <typename Container>
        void merge(Container &c, int left, int mid, int right,
                   typename Container::value_type *tmp)
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
                  typename Container::value_type *tmp)
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
        // bufor alokowany raz – nie robimy new w każdym wywołaniu rekurencyjnym
        typename Container::value_type *tmp = new typename Container::value_type[c.size()];
        detail::sort(c, 0, c.size() - 1, tmp);
        delete[] tmp;
    }
}
