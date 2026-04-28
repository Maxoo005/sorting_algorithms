#pragma once

// Merge sort 
// Złożoność czasowa: O(n log n)
// Złożoność pamięciowa: O(n) 

namespace MergeSort
{
    namespace detail
    {
        // szablonowa funkcja scalająca dwie posortowane połowy w jedną posortowaną całość
        template <typename Container>
        void merge(Container &c, int left, int mid, int right,
                   typename Container::value_type *tmp)
        {
            // Skopiuj zakres do bufora
            for (int i = left; i <= right; ++i)
                tmp[i] = c.get(i);

            int i = left;      // lewy iterator – zaczyna od poczatku lewej polowy
            int j = mid + 1;   // prawy iterator – zaczyna od poczatku prawej polowy
            int k = left;      // gdzie wpisujemy wynik z powrotem do c

            // glowna petla – porownujemy czolowe elementy obu polow i wstawiamy mniejszy
            while (i <= mid && j <= right)
            {
                if (tmp[i] <= tmp[j])   // <= zamiast < gwarantuje stabilnosc sortowania
                    c.set(k++, tmp[i++]);
                else
                    c.set(k++, tmp[j++]);
            }
            // jedna z polow sie skonczyla – dopisz reszte tej drugiej (juz posortowana)
            while (i <= mid)  c.set(k++, tmp[i++]);
            while (j <= right) c.set(k++, tmp[j++]);
        }

        template <typename Container>
        void sort(Container &c, int left, int right,
                  typename Container::value_type *tmp)
        {
            if (left >= right) return;  // 0 lub 1 element – nic do zrobienia
            // dzielic na pol: left + (right-left)/2 zamiast (left+right)/2 bo to chroni przed overflow
            int mid = left + (right - left) / 2;
            sort(c, left,    mid,   tmp);    // rekurencja na lewa polowe
            sort(c, mid + 1, right, tmp);    // rekurencja na prawa polowe
            merge(c, left, mid, right, tmp); // scalanie – tu dzieje sie wlasciwa robota
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
