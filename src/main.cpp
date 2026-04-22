#include "Parameters.h"
#include "utils/FileIO.hpp"
#include <iostream>


//declarations
static void runSingleFile();
static void runBenchmark();

//Tryby
static void runSingleFile()
{
    if (Parameters::inputFile.empty())
    {
        std::cerr << "Brak pliku wejściowego. Użyj -i <plik>.\n";
        return;
    }

    int size = 0;
    int *data = FileIO::load<int>(Parameters::inputFile, size);
    if (!data)
        return;

    // TODO: sortowanie danych (np. std::sort)

    if (!Parameters::outputFile.empty())
        FileIO::save<int>(Parameters::outputFile, data, size);

    delete[] data;
}

static void runBenchmark()
{
    // TODO: generowanie danych
    // TODO: wywołaj algorytm 
    // TODO: zapisz wyniki 
    std::cout << "benchmark mode – nie zaimplementowane\n";
}

//main
int main(int argc, char **argv)
{
    // Gotowa biblioteka – libparameters – zawiera funkcję readParameters
    //parsuje argumenty i ustawia globalną zmienną runMode
    if (Parameters::readParameters(argc - 1, argv + 1) != 0)
    {
        std::cerr << "Błąd parsowania parametrów. Użyj -h po pomoc.\n";
        return 1;
    }

    switch (Parameters::runMode)
    {
        case Parameters::RunModes::help:
            Parameters::help();
            break;

        case Parameters::RunModes::singleFile:
            runSingleFile();
            break;

        case Parameters::RunModes::benchmark:
            runBenchmark();
            break;

        default:
            std::cerr << "Nie podano trybu działania. Użyj -f, -b lub -h.\n";
            return 1;
    }

    return 0;
}
