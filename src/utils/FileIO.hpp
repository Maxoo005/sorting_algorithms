#pragma once
#include <fstream>
#include <iostream>
#include <string>

namespace FileIO
{
    // Wczytuje dane z pliku do tablicy 
    // pierwsza linia = liczba elementów N
    // kolejne N linii = wartości
    // wskaznik na tab i zapisanie rozwmiaru w outSize
    // błąd = nullptr outSize = 0  
    template <typename T>
    T *load(const std::string &path, int &outSize)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "FileIO: nie można otworzyć pliku: " << path << "\n";
            return nullptr;
        }

        int n = 0;
        file >> n;
        if (n <= 0)
        {
            std::cerr << "FileIO: nieprawidłowa liczba elementów: " << n << "\n";
            return nullptr;
        }

        T *data = new T[n];

        for (int i = 0; i < n; ++i)
        {
            if (!(file >> data[i]))
            {
                std::cerr << "FileIO: błąd odczytu elementu " << i << "\n";
                delete[] data;
                return nullptr;
            }
        }

        outSize = n;
        return data;
    }

    // tablica zapisana w tym samym formacie co load
    // true = sukces false = błąd
    template <typename T>
    bool save(const std::string &path, const T *data, int size)
    {
        if (!data || size <= 0)
        {
            std::cerr << "FileIO: brak danych do zapisu.\n";
            return false;
        }

        std::ofstream file(path);
        if (!file.is_open())
        {
            std::cerr << "FileIO: nie można otworzyć pliku do zapisu: " << path << "\n";
            return false;
        }

        file << size << "\n";
        for (int i = 0; i < size; ++i)
        {
            file << data[i] << "\n";
        }

        return true;
    }
}
