#include "Parameters.h"
#include "Runner.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    // libparameters — parsuje argumenty i ustawia globalny stan Parameters
    if (Parameters::readParameters(argc - 1, argv + 1) != 0)
    //argc -1 odejmuje potrzbea PRAWDZIWYCH argumentów 
    //argv + 1 przesuwa wskaźnik o 1 żeby pominąć nazwę programu
    {
        std::cerr << "Błąd parsowania parametrów. Użyj -h po helpp.\n";
        return 1;
    }

    Runner::run();

    return 0;
}
