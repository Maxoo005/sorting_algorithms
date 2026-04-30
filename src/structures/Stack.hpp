#pragma once

#include <stdexcept>

// Stos LIFO zbudowany na liście jednokierunkowej z ręcznym zarządzaniem pamięcią
// Interfejs get/set/swap/size/pushBack pozwala używać go z szablonowymi algorytmami sortowania
template <typename T>
class Stack
{
public:
    using value_type = T;

    struct Node
    {
        T     val;
        Node *next;
        explicit Node(const T &v) : val(v), next(nullptr) {}
    };

    Stack() : m_top(nullptr), m_size(0) {}

    ~Stack()
    {
        while (m_top)
        {
            Node *next = m_top->next;
            delete m_top;
            m_top = next;
        }
    }

    Stack(const Stack &) = delete;
    Stack &operator=(const Stack &) = delete;

    // push = dodaj na szczyt
    void push(const T &val)
    {
        Node *n  = new Node(val);
        n->next  = m_top;
        m_top    = n;
        ++m_size;
    }

    // pushBack – wymagane przez Runner dodaje elementy tak jak push
    void pushBack(const T &val) { push(val); }

    // dostęp indeksowy: 0 = szczyt, size-1 = dno – O(n)
    T &get(int i)             { return nodeAt(i)->val; }
    const T &get(int i) const { return nodeAt(i)->val; }

    void set(int i, const T &val) { nodeAt(i)->val = val; }

    void swap(int i, int j)
    {
        // zamiast chodzić dwa razy – wyciągnij wskaźniki i zamień wartości
        T &a = nodeAt(i)->val;
        T &b = nodeAt(j)->val;
        T  tmp = a;
        a = b;
        b = tmp;
    }

    int size() const { return m_size; }

private:
    Node *m_top;
    int   m_size;

    Node *nodeAt(int i) const
    {
        if (i < 0 || i >= m_size)
            throw std::out_of_range("Stack: index out of range");
        Node *cur = m_top;
        for (int k = 0; k < i; ++k) cur = cur->next;
        return cur;
    }
};
