#pragma once
//ifndef guard
#include <chrono>

// Czas w mikrosekundach
//   Timer t
//   t.start()
//   sort(data, size)
//   t.stop()
//   long long us = t.elapsed()

class Timer
{
public:
    void start()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        m_end = std::chrono::high_resolution_clock::now();
    }

    //zwraca czas w mikrosekundach
    long long elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_begin).count();
    }
//prosty ale podobno dokładny timer oparty na std::chrono
private:
    std::chrono::high_resolution_clock::time_point m_begin;
    std::chrono::high_resolution_clock::time_point m_end;
};
