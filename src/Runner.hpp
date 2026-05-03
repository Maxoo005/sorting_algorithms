#pragma once

#include "Parameters.h"
#include "utils/FileIO.hpp"
#include "utils/Verify.hpp"
#include "utils/Timer.hpp"
#include "utils/CsvLogger.hpp"
#include "structures/SortArray.hpp"
#include "structures/SingleList.hpp"
#include "structures/DoubleList.hpp"
#include "structures/Stack.hpp"
#include "algorithms/MergeSort.hpp"
#include "algorithms/QuickSort.hpp"
#include "algorithms/BucketSort.hpp"
#include <algorithm>
#include <climits>
#include <iostream>
#include <limits>
#include <random>

// Glowna klasa sterujaca programem.
// Metody statyczne - nie tworzymy obiektu Runner, wolamy Runner::run().
// Dispatch po typie danych (T) i strukturze odbywa sie przez szablony.
class Runner
{
public:
    static void run()
    {
        switch (Parameters::runMode)
        {
            case Parameters::RunModes::help:       Parameters::help(); break;
            case Parameters::RunModes::singleFile: dispatchType_singleFile(); break;
            case Parameters::RunModes::benchmark:  dispatchType_benchmark();  break;
            default:
                std::cerr << "Nie podano trybu dzialania. Uzyj -f, -b lub -h.\n";
                break;
        }
    }

private:
    // Walidacja algorytmu - jedna funkcja dla obu trybow
    static bool algorithmSupported()
    {
        switch (Parameters::algorithm)
        {
            case Parameters::Algorithms::merge:
            case Parameters::Algorithms::quick:
            case Parameters::Algorithms::bucket:
                return true;
            default:
                std::cerr << "Nieobslugiwany algorytm. Uzyj -a 2 (merge), -a 4 (bucket), -a 5 (quick).\n";
                return false;
        }
    }

    // Dispatch po dataType - wywoluje odpowiednia specjalizacje szablonu
    static void dispatchType_singleFile()
    {
        switch (Parameters::dataType)
        {
            case Parameters::DataTypes::typeFloat:       singleFile<float>();        break;
            case Parameters::DataTypes::tyleUnsignedInt: singleFile<unsigned int>(); break;
            case Parameters::DataTypes::typeChar:        singleFile<char>();         break;
            default:                                     singleFile<int>();          break; // typeInt + undefined
        }
    }

    static void dispatchType_benchmark()
    {
        switch (Parameters::dataType)
        {
            case Parameters::DataTypes::typeFloat:       benchmark<float>();        break;
            case Parameters::DataTypes::tyleUnsignedInt: benchmark<unsigned int>(); break;
            case Parameters::DataTypes::typeChar:        benchmark<char>();         break;
            default:                                     benchmark<int>();          break;
        }
    }

    // Tryb a) - wczytaj plik, posortuj, opcjonalnie zapisz
    template <typename T>
    static void singleFile()
    {
        if (Parameters::inputFile.empty())
        {
            std::cerr << "Brak pliku wejsciowego. Uzyj -i <plik>.\n";
            return;
        }
        if (!algorithmSupported()) return;

        int  size = 0;
        T   *data = FileIO::load<T>(Parameters::inputFile, size);
        if (!data) return;

        switch (Parameters::structure)
        {
            case Parameters::Structures::array:
            {
                SortArray<T> arr(data, size);
                sortDispatch(arr);
                for (int i = 0; i < size; ++i) data[i] = arr.get(i);
                break;
            }
            case Parameters::Structures::singleList:
            {
                SingleList<T> list;
                for (int i = 0; i < size; ++i) list.pushBack(data[i]);
                sortDispatch(list);
                for (int i = 0; i < size; ++i) data[i] = list.get(i);
                break;
            }
            case Parameters::Structures::doubleList:
            {
                DoubleList<T> list;
                for (int i = 0; i < size; ++i) list.pushBack(data[i]);
                sortDispatch(list);
                for (int i = 0; i < size; ++i) data[i] = list.get(i);
                break;
            }
            case Parameters::Structures::stack:
            {
                Stack<T> stk;
                for (int i = 0; i < size; ++i) stk.pushBack(data[i]);
                sortDispatch(stk);
                for (int i = 0; i < size; ++i) data[i] = stk.get(i);
                break;
            }
            default:
                std::cerr << "singleFile: nieobslugiwana struktura. Uzyj -s 0/1/2/4.\n";
                delete[] data;
                return;
        }

        Verify::check(data, size);

        if (!Parameters::outputFile.empty())
            FileIO::save<T>(Parameters::outputFile, data, size);

        delete[] data;
    }

    // Tryb b) - benchmark: N powtorzen, kazdy pomiar zapisywany do CSV
    template <typename T>
    static void benchmark()
    {
        if (!algorithmSupported()) return;

        const int n    = Parameters::structureSize;
        const int reps = Parameters::iterations;

        if (n <= 0)
        {
            std::cerr << "benchmark: rozmiar struktury musi byc > 0 (-l <rozmiar>).\n";
            return;
        }
        if (reps <= 0)
        {
            std::cerr << "benchmark: liczba iteracji musi byc > 0 (-n <iteracje>).\n";
            return;
        }

        // Generuj dane raz - kazda iteracja sortuje TEN SAM zestaw (sortOnce kopiuje wewnetrznie)
        T *src = generateData<T>(n);

        long long minTime = LLONG_MAX;
        long long maxTime = LLONG_MIN;
        long long sumTime = 0LL;

        for (int iter = 0; iter < reps; ++iter)
        {
            const long long us = sortOnce<T>(src, n);

            if (us < 0)
            {
                delete[] src;
                return;
            }

            if (us < minTime) minTime = us;
            if (us > maxTime) maxTime = us;
            sumTime += us;

            if (!Parameters::resultsFile.empty())
                CsvLogger::append(Parameters::resultsFile, us);
        }

        delete[] src;

        const long long avgTime = sumTime / reps;
        std::cout << "min=" << minTime << "us  avg=" << avgTime
                  << "us  max=" << maxTime << "us\n";
    }

    // Generowanie danych wg rozkladu - szablon obsluguje int/float/uint/char
    template <typename T>
    static T *generateData(int n)
    {
        T *data = new T[n];
        std::mt19937 rng(std::random_device{}());
        // uniform_int_distribution wymaga calkowitoliczbowego T
        // dla float uzywamy uniform_real_distribution
        fillRandom(data, n, rng);

        switch (Parameters::distribution)
        {
            case Parameters::Distribution::ascending:
                std::sort(data, data + n);
                break;
            case Parameters::Distribution::descending:
                std::sort(data, data + n, std::greater<T>());
                break;
            case Parameters::Distribution::ascending50Per:
                std::sort(data, data + n / 2);
                break;
            default: // random
                break;
        }

        return data;
    }

    // Losowe wypelnianie - osobne przeciazenia dla float i calkowitoliczbowych
    template <typename T>
    static void fillRandom(T *data, int n, std::mt19937 &rng)
    {
        // domyslne przeciazenie: typy calkowite (int, unsigned int)
        std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(),
                                              std::numeric_limits<T>::max());
        for (int i = 0; i < n; ++i)
            data[i] = dist(rng);
    }

    static void fillRandom(char *data, int n, std::mt19937 &rng)
    {
        std::uniform_int_distribution<short> dist(32, 126); // printable ASCII
        for (int i = 0; i < n; ++i)
            data[i] = static_cast<char>(dist(rng));
    }

    static void fillRandom(float *data, int n, std::mt19937 &rng)
    {
        // float: pelny zakres +-3.4e38 bylby numerycznie problematyczny dla BucketSort
        std::uniform_real_distribution<float> dist(-1.0e6f, 1.0e6f);
        for (int i = 0; i < n; ++i)
            data[i] = dist(rng);
    }

    // Jeden przebieg sortowania - kopiuje dane do struktury, mierzy czas
    // Zwraca czas w us lub -1 przy bledzie
    template <typename T>
    static long long sortOnce(const T *src, int n)
    {
        Timer t;

        switch (Parameters::structure)
        {
            case Parameters::Structures::array:
            {
                SortArray<T> arr(src, n);
                t.start(); sortDispatch(arr); t.stop();
                break;
            }
            case Parameters::Structures::singleList:
            {
                SingleList<T> list;
                for (int i = 0; i < n; ++i) list.pushBack(src[i]);
                t.start(); sortDispatch(list); t.stop();
                break;
            }
            case Parameters::Structures::doubleList:
            {
                DoubleList<T> list;
                for (int i = 0; i < n; ++i) list.pushBack(src[i]);
                t.start(); sortDispatch(list); t.stop();
                break;
            }
            case Parameters::Structures::stack:
            {
                Stack<T> stk;
                for (int i = 0; i < n; ++i) stk.pushBack(src[i]);
                t.start(); sortDispatch(stk); t.stop();
                break;
            }
            default:
                std::cerr << "sortOnce: nieobslugiwana struktura. Uzyj -s 0/1/2/4.\n";
                return -1;
        }

        return t.elapsed();
    }

    // Dispatch algorytmu - jeden punkt dla wszystkich kombinacji
    template <typename Container>
    static void sortDispatch(Container &c)
    {
        switch (Parameters::algorithm)
        {
            case Parameters::Algorithms::merge:  MergeSort::sort(c);  break;
            case Parameters::Algorithms::quick:  QuickSort::sort(c);  break;
            case Parameters::Algorithms::bucket: BucketSort::sort(c); break;
            default:
                std::cerr << "sortDispatch: nieobslugiwany algorytm.\n";
                break;
        }
    }
};
