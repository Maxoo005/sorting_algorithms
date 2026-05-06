#pragma once

#include <cassert>

// Lista jednokierunkowa z interfejsem dla algorytmów sortowania.
// Dostęp do i-tego elementu O(n), swap wartości O(n).
template <typename T>
class SingleList
{
public:
    using value_type = T;

    struct Node
    {
        T     val;
        Node *next;  // wskaznik tylko do przodu, wstecz sie nie cofniemy
        explicit Node(const T &v) : val(v), next(nullptr) {}
    };

    SingleList() : m_head(nullptr), m_size(0) {}

    ~SingleList()
    {
        while (m_head)
        {
            Node *next = m_head->next; // zapamietaj nastepny ZANIM zwolnisz aktualny
            delete m_head;
            m_head = next;
        }
    }

    SingleList(const SingleList &) = delete;
    SingleList &operator=(const SingleList &) = delete;

    // Wstawia element na koniec – używane przy wczytywaniu danych
    // trick z Node** pozwala uniknąć osobnego sprawdzania "czy lista pusta"
    void pushBack(const T &val)
    {
        Node **cur = &m_head;          // cur wskazuje na pole które chcemy wypełnić
        while (*cur) cur = &(*cur)->next; // idź do ostatniego nullptr
        *cur = new Node(val);          // wstaw tu nowy wezel
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
        assert(i >= 0 && i < m_size); // index out of range
        Node *cur = m_head;
        for (int k = 0; k < i; ++k) cur = cur->next; // zawsze od glowy, O(n)
        return cur;
    }
};
