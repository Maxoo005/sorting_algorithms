#pragma once

#include "Parameters.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// dipisuje do wyników csv
//data;tryb;algorytm;struktura;typ;rozkład;rozmiar;iteracje;pivot;shellParam;min;avg;max
namespace CsvLogger
{
    namespace detail
    {
        inline std::string timestamp()          //zwykła data i czas
        {
            auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);   //wersja windowsa
#else
            localtime_r(&t, &tm);   //wersja linuxa
#endif
            char buf[32];           //format daty czasu i czytelny zapisaz
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
            return buf;
        }

        inline void writeHeader(std::ofstream &f)
        {
            f << "timestamp"
              << ";runMode"
              << ";algorithm"
              << ";structure"
              << ";dataType"
              << ";distribution"
              << ";structureSize"
              << ";iterations"
              << ";pivot"
              << ";shellParam"
              << ";min_us"
              << ";avg_us"
              << ";max_us"
              << "\n";
        }
    }

    
    // jak pliku ni ema to go stworzy
    inline bool append(const std::string &path,
                       long long min_us,
                       long long avg_us,
                       long long max_us)
    {
        if (path.empty())
        {
            std::cerr << "CsvLogger: brak ścieżki do pliku wyników.\n";
            return false;
        }

        // czy istnieje
        bool needsHeader = false;
        {
            std::ifstream check(path);
            needsHeader = !check.is_open();
        }

        std::ofstream f(path, std::ios::app);   //dopisywanie
        if (!f.is_open())
        {
            std::cerr << "CsvLogger: nie można otworzyć pliku: " << path << "\n";
            return false;
        }

        if (needsHeader)            //nagłówek tylko dla nowego pliku
            detail::writeHeader(f);

        f << detail::timestamp()            //bez << enum nie ogarnie zapisuje prametry i wyniki
          << ";" << static_cast<int>(Parameters::runMode)
          << ";" << static_cast<int>(Parameters::algorithm)
          << ";" << static_cast<int>(Parameters::structure)
          << ";" << static_cast<int>(Parameters::dataType)
          << ";" << static_cast<int>(Parameters::distribution)
          << ";" << Parameters::structureSize
          << ";" << Parameters::iterations
          << ";" << static_cast<int>(Parameters::pivot)
          << ";" << static_cast<int>(Parameters::shellParameter)
          << ";" << min_us
          << ";" << avg_us
          << ";" << max_us
          << "\n";

        return true;
    }
}
