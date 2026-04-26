#pragma once

#include <algorithm>
#include <stdexcept>

// Lista dwukierunkowa z interfejsem algorytmów 
// Dostęp do i-tego elementu O(n/2), swap wartości O(n).
template <typename T>
class DoubleList
{
public:
    struct Node
    {
        T     val;
        Node *prev;
        Node *next;
        explicit Node(const T &v) : val(v), prev(nullptr), next(nullptr) {}
    };

    DoubleList() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

    ~DoubleList()
    {
        while (m_head)
        {
            Node *next = m_head->next;
            delete m_head;
            m_head = next;
        }
    }

    DoubleList(const DoubleList &) = delete;
    DoubleList &operator=(const DoubleList &) = delete;

    // myt element na koniec
    void pushBack(const T &val)
    {
        Node *n = new Node(val);
        if (!m_tail) { m_head = m_tail = n; }
        else { n->prev = m_tail; m_tail->next = n; m_tail = n; }
        ++m_size;
    }

    T &get(int i)       { return nodeAt(i)->val; }
    void set(int i, const T &val) { nodeAt(i)->val = val; }

    void swap(int i, int j) { std::swap(get(i), get(j)); }

    int   size()  const { return m_size; }
    Node *head()        { return m_head; }
    Node *tail()        { return m_tail; }

private:
    Node *m_head;
    Node *m_tail;
    int   m_size;

    // do najbliższej strony pocz lub koniea
    Node *nodeAt(int i) const
    {
        if (i < 0 || i >= m_size)
            throw std::out_of_range("DoubleList: index out of range");

        if (i < m_size / 2)
        {
            Node *cur = m_head;
            for (int k = 0; k < i; ++k) cur = cur->next;
            return cur;
        }
        else
        {
            Node *cur = m_tail;
            for (int k = m_size - 1; k > i; --k) cur = cur->prev;
            return cur;
        }
    }
};
