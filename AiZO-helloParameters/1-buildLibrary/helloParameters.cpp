#include "helloParameters.h"
#include <iostream>

void helloWorld()
{
  std::cout << "Hello AiZO!\n";
}

int printArguments(int argc, char **argv)
{
  if (argc <= 0)
  {
    std::cout << "ERROR! No arguments has been passed!\n";

    return -1;
  }

  for (int i = 0; i < argc; i++)
  {
    std::cout << "Parameter: " << i << " - " << argv[i] << "\n";
  }

  return 0;
}
