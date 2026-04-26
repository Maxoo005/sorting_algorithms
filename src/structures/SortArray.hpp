#pragma once

#include <algorithm>
#include <vector>

// Tablica dynamiczna z interfejsem dla alg sortowania
// Dostęp losowy O(1), swap O(1)
template <typename T>
class SortArray
{
public:
    using value_type = T;

    explicit SortArray(int capacity) : m_data(capacity) {}

    SortArray(const T *src, int size) : m_data(src, src + size) {}

    int      size()                    const { return static_cast<int>(m_data.size()); }
    T       &operator[](int i)               { return m_data[i]; }
    const T &operator[](int i)         const { return m_data[i]; }
    void     swap(int i, int j)              { std::swap(m_data[i], m_data[j]); }

    // surowy wskaźnik do danych
    //T       *raw()       { return m_data.data(); }
    //const T *raw() const { return m_data.data(); }

private:
    std::vector<T> m_data;
};
