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
    template <typename T>   //działa na int float double
    T *load(const std::string &path, int &outSize)   //surowy wskaźnik na tablice
    {
        std::ifstream file(path);
        if (!file.is_open())            //odczyt do pliku
        {
            std::cerr << "FileIO: nie można otworzyć pliku: " << path << "\n";
            return nullptr;
        }
            //pierwsza linia to liczba elementów potem dalej
        int n = 0;
        file >> n;
        if (n <= 0)
        {
            std::cerr << "FileIO: nieprawidłowa liczba elementów: " << n << "\n";
            return nullptr;
        }

        T *data = new T[n];     //alokacja tablicy rozmiar znany podczas wykoannia

        for (int i = 0; i < n; ++i)             // wczytuje element po elemencie
        {
            if (!(file >> data[i]))
            {
                std::cerr << "FileIO: błąd odczytu elementu " << i << "\n";
                delete[] data;
                return nullptr;
            }
        }
        //zapis rozmiaru i zwrot tablicy
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
