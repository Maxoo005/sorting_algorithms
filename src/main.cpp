#include "Parameters.h"
#include "Runner.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    // libparameters — parsuje argumenty i ustawia globalny stan Parameters
    if (Parameters::readParameters(argc - 1, argv + 1) != 0)
    {
        std::cerr << "Błąd parsowania parametrów. Użyj -h po pomoc.\n";
        return 1;
    }

    Runner::run();

    return 0;
}
