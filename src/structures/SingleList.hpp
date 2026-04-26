#pragma once

#include <stdexcept>

// Lista jednokierunkowa z interfejsem dla algorytmów sortowania.
// Dostęp do i-tego elementu O(n), swap wartości O(n).
template <typename T>
class SingleList
{
public:
    struct Node
    {
        T     val;
        Node *next;
        explicit Node(const T &v) : val(v), next(nullptr) {}
    };

    SingleList() : m_head(nullptr), m_size(0) {}

    ~SingleList()
    {
        while (m_head)
        {
            Node *next = m_head->next;
            delete m_head;
            m_head = next;
        }
    }

    SingleList(const SingleList &) = delete;
    SingleList &operator=(const SingleList &) = delete;

    // Wstawia element na koniec – używane przy wczytywaniu danych
    void pushBack(const T &val)
    {
        Node **cur = &m_head;
        while (*cur) cur = &(*cur)->next;
        *cur = new Node(val);
        ++m_size;
    }

    T &get(int i)       { return nodeAt(i)->val; }
    void set(int i, const T &val) { nodeAt(i)->val = val; }

    void swap(int i, int j) { std::swap(get(i), get(j)); }

    int   size()  const { return m_size; }
    Node *head()        { return m_head; }

private:
    Node *m_head;
    int   m_size;

    Node *nodeAt(int i) const
    {
        if (i < 0 || i >= m_size)
            throw std::out_of_range("SingleList: index out of range");
        Node *cur = m_head;
        for (int k = 0; k < i; ++k) cur = cur->next;
        return cur;
    }
};
