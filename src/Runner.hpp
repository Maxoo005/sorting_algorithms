#pragma once

#include "Parameters.h"
#include "utils/FileIO.hpp"
#include "utils/Verify.hpp"
#include "utils/Timer.hpp"
#include "utils/CsvLogger.hpp"
#include "structures/SortArray.hpp"
#include "structures/SingleList.hpp"
#include "structures/DoubleList.hpp"
#include "algorithms/MergeSort.hpp"
#include <algorithm>
#include <climits>
#include <iostream>
#include <random>

// Wywołaj Runner::run() / Parameters::readParameters.
class Runner
{
public:
    // wejście Parameters::runMode.
    static void run()
    {
        switch (Parameters::runMode)
        {
            case Parameters::RunModes::help:
                Parameters::help();
                break;

            case Parameters::RunModes::singleFile:
                singleFile();
                break;

            case Parameters::RunModes::benchmark:
                benchmark();
                break;

            default:
                std::cerr << "Nie podano trybu działania. Użyj -f, -b lub -h.\n";
                break;
        }
    }

private:
    // Tryb a) — sortuje dane z pliku i opcjonalnie zapisuje wynik do pliku
    static void singleFile()
    {
        if (Parameters::inputFile.empty())
        {
            std::cerr << "Brak pliku wejściowego. Użyj -i <plik>.\n";
            return;
        }

        if (Parameters::algorithm != Parameters::Algorithms::merge)
        {
            std::cerr << "singleFile: tylko MergeSort jest zaimplementowany (-a 2).\n";
            return;
        }

        int  size = 0;
        int *data = FileIO::load<int>(Parameters::inputFile, size);
        if (!data)
            return;

        switch (Parameters::structure)
        {
            case Parameters::Structures::array:
            {
                SortArray<int> arr(data, size);
                MergeSort::sort(arr);
                for (int i = 0; i < size; ++i) data[i] = arr.get(i);
                break;
            }
            case Parameters::Structures::singleList:
            {
                SingleList<int> list;
                for (int i = 0; i < size; ++i) list.pushBack(data[i]);
                MergeSort::sort(list);
                for (int i = 0; i < size; ++i) data[i] = list.get(i);
                break;
            }
            case Parameters::Structures::doubleList:
            {
                DoubleList<int> list;
                for (int i = 0; i < size; ++i) list.pushBack(data[i]);
                MergeSort::sort(list);
                for (int i = 0; i < size; ++i) data[i] = list.get(i);
                break;
            }
            default:
                std::cerr << "singleFile: nieobsługiwana struktura. Użyj -s 0/1/2.\n";
                delete[] data;
                return;
        }

        Verify::check(data, size);

        if (!Parameters::outputFile.empty())
            FileIO::save<int>(Parameters::outputFile, data, size);

        delete[] data;
    }

    // Tryb b) — wielokrotne sortowanie tych samych danych, zapis wyników do CSV.
    static void benchmark()
    {
        if (Parameters::algorithm != Parameters::Algorithms::merge)
        {
            std::cerr << "benchmark: tylko MergeSort jest zaimplementowany (-a 2).\n";
            return;
        }

        const int n    = Parameters::structureSize;
        const int reps = Parameters::iterations;

        if (n <= 0)
        {
            std::cerr << "benchmark: rozmiar struktury musi być > 0 (-n <rozmiar>).\n";
            return;
        }
        if (reps <= 0)
        {
            std::cerr << "benchmark: liczba iteracji musi być > 0 (-c <iteracje>).\n";
            return;
        }

        // Generuj dane raz wszystkie iteracje używają tego samego wejścia
        int *src = generateData(n);

        long long minTime = LLONG_MAX;
        long long maxTime = LLONG_MIN;
        long long sumTime = 0LL;

        for (int iter = 0; iter < reps; ++iter)
        {
            // sortOnce kopiuje src do struktury wewnętrznie src pozostaje niezmienione
            const long long us = sortOnce(src, n);

            if (us < 0)
            {
                delete[] src;
                return; // błąd struktury 
            }

            if (us < minTime) minTime = us;
            if (us > maxTime) maxTime = us;
            sumTime += us;
        }

        delete[] src;

        const long long avgTime = sumTime / reps;

        std::cout << "min=" << minTime << "us  avg=" << avgTime
                  << "us  max=" << maxTime << "us\n";

        if (!Parameters::resultsFile.empty())
            CsvLogger::append(Parameters::resultsFile, minTime, avgTime, maxTime);
    }

    // Generuje tablicę int[n] wg Parameters::distribution.
    // Wstępne sortowanie wykonywane przez std::sort
    static int *generateData(int n)
    {
        int *data = new int[n];

        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

        for (int i = 0; i < n; ++i)
            data[i] = dist(rng);

        switch (Parameters::distribution)
        {
            case Parameters::Distribution::ascending:
                std::sort(data, data + n);
                break;

            case Parameters::Distribution::descending:
                std::sort(data, data + n, std::greater<int>());
                break;

            case Parameters::Distribution::ascending50Per:
                std::sort(data, data + n / 2);
                break;

            default: // random
                break;
        }

        return data;
    }

    // Kopiuje src do wybranej struktury sortuje MergeSortem i zwraca czas 
    // Zwraca -1 przy nieobsługiwanej strukturze
    static long long sortOnce(const int *src, int n)
    {
        Timer t;

        switch (Parameters::structure)
        {
            case Parameters::Structures::array:
            {
                SortArray<int> arr(src, n);
                t.start();
                MergeSort::sort(arr);
                t.stop();
                break;
            }
            case Parameters::Structures::singleList:
            {
                SingleList<int> list;
                for (int i = 0; i < n; ++i) list.pushBack(src[i]);
                t.start();
                MergeSort::sort(list);
                t.stop();
                break;
            }
            case Parameters::Structures::doubleList:
            {
                DoubleList<int> list;
                for (int i = 0; i < n; ++i) list.pushBack(src[i]);
                t.start();
                MergeSort::sort(list);
                t.stop();
                break;
            }
            default:
                std::cerr << "sortOnce: nieobsługiwana struktura. Użyj -s 0/1/2.\n";
                return -1;
        }

        return t.elapsed();
    }
};
