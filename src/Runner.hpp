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

// Główna klasa sterująca programem
// Wszystkie metody są statyczne – nie tworzymy obiektu Runner, po prostu wołamy Runner::run()
class Runner
{
public:
    // Punkt wejścia – czyta Parameters::runMode i uruchamia odpowiedni tryb
    // to tylko kontener, nie tworzy obiektu Runner, więc wszystko jest static
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
    // Tryb a) — wczytuje dane z pliku, sortuje i opcjonalnie zapisuje wynik
    // struktura wybierana przez Parameters::structure (array/singleList/doubleList)
    static void singleFile()
    {
        if (Parameters::inputFile.empty()) //walidacja bez pliku ni ma co
        {
            std::cerr << "Brak pliku wejściowego. Użyj -i <plik>.\n";
            return;
        }

        if (Parameters::algorithm != Parameters::Algorithms::merge) //mój strażnik tylko merge narazie
        {
            std::cerr << "singleFile: tylko MergeSort jest zaimplementowany (-a 2).\n";
            return;
        }
            //fielO nullptr przy bledzie 
        int  size = 0;
        int *data = FileIO::load<int>(Parameters::inputFile, size); // wczytaj do surowej tablicy
        if (!data)
            return;

        switch (Parameters::structure)
        {

            /*powtarza sie
                1zbuduj
                2porostuj
                3przepisz wynik do tab data jest co veryfikować
            */
            case Parameters::Structures::array:
            {
                SortArray<int> arr(data, size); // kopiuje data do SortArray
                MergeSort::sort(arr);
                for (int i = 0; i < size; ++i) data[i] = arr.get(i); // przepisz wynik z powrotem
                break;
            }
            case Parameters::Structures::singleList:
            {
                SingleList<int> list;
                for (int i = 0; i < size; ++i) list.pushBack(data[i]); // zbuduj liste z danych
                MergeSort::sort(list);
                for (int i = 0; i < size; ++i) data[i] = list.get(i); // przepisz wynik
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

        Verify::check(data, size); // sprawdza czy tablica jest posortowana (assert/debug)
            //zapis do plik jesli podane -o i zwolnienie pamieci
        if (!Parameters::outputFile.empty())
            FileIO::save<int>(Parameters::outputFile, data, size);

        delete[] data; // pamięć zaalokowana przez FileIO::load, zwalniamy
    }

    // Tryb b) — benchmark: sortuje te same dane reps razy i mierzy czas
    // wyniki (min/avg/max) trafiają na stdout i opcjonalnie do pliku CSV
    static void benchmark()
    {
        if (Parameters::algorithm != Parameters::Algorithms::merge)
        {
            std::cerr << "benchmark: tylko MergeSort jest zaimplementowany (-a 2).\n";
            return;
        }
        //zeby nie pisac caly czas parameters::
        const int n    = Parameters::structureSize;
        const int reps = Parameters::iterations;

        if (n <= 0)
        {
            std::cerr << "benchmark: rozmiar struktury musi być > 0 (-l <rozmiar>).\n";
            return;
        }
        if (reps <= 0)
        {
            std::cerr << "benchmark: liczba iteracji musi być > 0 (-n <iteracje>).\n";
            return;
        }

        // generuj dane raz – każda iteracja sortuje TEN SAM zestaw (sortOnce kopiuje src wewnętrznie)
        // dzięki temu wyniki są porównywalne między iteracjami
        int *src = generateData(n);

        long long minTime = LLONG_MAX;
        long long maxTime = LLONG_MIN;
        long long sumTime = 0LL;

        for (int iter = 0; iter < reps; ++iter) //-1 blad
        {
            // sortOnce kopiuje src do struktury wewnętrznie, src pozostaje niezmienione
            const long long us = sortOnce(src, n); // czas w mikrosekundach

            if (us < 0)
            {
                delete[] src;
                return; // błąd struktury
            }

            // zbieraj statystyki
            if (us < minTime) minTime = us;
            if (us > maxTime) maxTime = us;
            sumTime += us;
        }

        delete[] src;

        const long long avgTime = sumTime / reps;

        std::cout << "min=" << minTime << "us  avg=" << avgTime
                  << "us  max=" << maxTime << "us\n";

        if (!Parameters::resultsFile.empty())  //zapis
            CsvLogger::append(Parameters::resultsFile, minTime, avgTime, maxTime);
    }

    // Generuje int[n] wg Parameters::distribution
    // random: std::mt19937 (Mersenne Twister) – szybszy i lepszy niż rand()
    // ascending/descending: po wygenerowaniu losowym sortuje std::sort
    // ascending50Per: sortuje tylko pierwszą połowę, reszta losowa
    static int *generateData(int n)
    {
        int *data = new int[n];
                //niby dobry szybki i deteriministyczny generator liczb losowych
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
            //wypełnij tablicę losowymi wartościami
        for (int i = 0; i < n; ++i)
            data[i] = dist(rng);
                    //rozkład danych na wygenerowanej losowej tablicy
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

    // Kopiuje src do wybranej struktury, sortuje MergeSortem i zwraca czas w us
    // Timer startuje DOPIERO po skopiowaniu – mierzymy tylko czas sortowania, nie budowania struktury
    // Zwraca -1 przy nieobsługiwanej strukturze
    static long long sortOnce(const int *src, int n)
    {
        Timer t;    //timer lokalny, nie start 

        switch (Parameters::structure)
        {
            case Parameters::Structures::array:
            {
                SortArray<int> arr(src, n); // kopia danych
                t.start();
                MergeSort::sort(arr);
                t.stop();
                break;
            }
            case Parameters::Structures::singleList:
            {
                SingleList<int> list;
                for (int i = 0; i < n; ++i) list.pushBack(src[i]); // budowanie listy NIE jest mierzone
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

        return t.elapsed();   //zwraca czas w mikrosekundach
        
    }
};
