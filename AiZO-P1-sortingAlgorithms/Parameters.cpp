#include "Parameters.h"
#include <iostream>
#include <vector>

#define LOG(x) "[" #x "=" << (x) << "]"
#define LOG_ENUM(x) "[" #x "=" << static_cast<int>((x)) << "]"

namespace Parameters
{

//
// Setting default values for parameters.
//

RunModes runMode = RunModes::undefined;
Algorithms algorithm = Algorithms::undefined;
Structures structure = Structures::undefined;
DataTypes dataType = DataTypes::undefined;

std::string inputFile = {};
std::string outputFile = {};
std::string resultsFile = {};

Distribution distribution = Distribution::undefined;

int structureSize = -1;
int iterations = -1;

Pivots pivot = Pivots::undefined;
ShellParameters shellParameter = ShellParameters::undefined;

//
// Additional internal variables.
//

bool debug = false;
bool errorFlag = false;

bool check(std::string parameter, std::string key1,
               std::string key2 = {}, std::string key3 = {},
                   std::string key4 = {}, std::string key5 = {})
{
  std::vector<std::string> keys {key1, key2, key3, key4, key5};

  for (size_t i = 0; i < keys.size(); i++)
  {
    if (parameter == keys[i])
    {
      return true;
    }
  }

  return false;
}

template <typename T>
int updateEnumParameter(T &parameter, T count, std::string valueStr)
{
  int newValue = std::stoi(valueStr);

  if ((newValue < 0) ||
          (newValue >= static_cast<int>(count)))
  {
    std::cerr << "ERROR! Value is out of enum bounds!\n";

    errorFlag = true;
    return -1;
  }

  parameter = static_cast<T>(newValue);

  return 0;
}

int readParameters(int argc, char **argv)
{
  if (argc <= 0)
  {
    std::cerr << "ERROR! No parameters or wrong number!\n";
    return -1;
  }

  for (int i = 0; i < argc; i++)
  {
    std::string arg = std::string(argv[i]);

    if (debug)
    {
      std::cout << "DEBUG: Analysing parameter on index " << LOG(i)
                << " " << LOG(arg) << ".\n";
    }

    //
    // Run modes.
    //

    if (check(arg, "--singleFile", "-f"))
    {
      runMode = RunModes::singleFile;
      continue;
    }
    else if (check(arg, "--benchmark", "-b"))
    {
      runMode = RunModes::benchmark;
      continue;
    }
    else if (check(arg, "--help", "-h"))
    {
      runMode = RunModes::help;
      continue;
    }

    //
    // Parmeters that require passed value.
    //
    // Check if you can safely use the next value.
    //

    if (i + 1 >= argc)
    {
      std::cerr << "ERROR! No value has been passed for "
                << LOG(arg) << " or parameter name is wrong!\n";
      return -1;
    }

    std::string value = std::string(argv[++i]);

    if (debug)
    {
      std::cout << "DEBUG: Possible new value for " << LOG(arg)
                << " will be " << LOG(value) << ".\n";
    }

    if (check(arg, "--algorithm", "-a"))
    {
      updateEnumParameter(algorithm, Algorithms::count, value);
      continue;
    }

    if (check(arg, "--structure", "-s"))
    {
      updateEnumParameter(structure, Structures::count, value);
      continue;
    }

    if (check(arg, "--dataType", "-t"))
    {
      updateEnumParameter(dataType, DataTypes::count, value);
      continue;
    }

    if (check(arg, "--inputFile", "-i"))
    {
      inputFile = value;
      continue;
    }

    if (check(arg, "--outputFile", "-o"))
    {
      outputFile = value;
      continue;
    }

    if (check(arg, "--resultsFile", "-r"))
    {
      resultsFile = value;
      continue;
    }

    if (check(arg, "--distribution", "-d"))
    {
      updateEnumParameter(distribution, Distribution::count, value);
      continue;
    }

    if (check(arg, "--structureSize", "-l"))
    {
      structureSize = std::stoi(value);
      continue;
    }

    if (check(arg, "--iterations", "-n"))
    {
      iterations = std::stoi(value);
      continue;
    }

    //
    // Parameters for specific algorithms.
    //

    if (check(arg, "--pivot", "-p"))
    {
      updateEnumParameter(pivot, Pivots::count, value);
      continue;
    }

    if (check(arg, "--shellParameter", "-e"))
    {
      updateEnumParameter(shellParameter, ShellParameters::count, value);
      continue;
    }

    //
    // Unknown parameter.
    //

    std::cerr << "ERROR! Parameter " << LOG(arg) << " was not recognized!\n";
    return -1;
  }

  if (errorFlag)
  {
    std::cerr << "ERROR! Something went wrong with reading parameters!\n";
    return -1;
  }

  return 0;
}

void enableDebug()
{
  std::cout << "DEBUG: Parameters debugging enabled.\n";
  debug = true;
}

void disableDebug()
{
  std::cout << "DEBUG: Parameters debugging disabled.\n";
  debug = false;
}

void help()
{
  std::cout << "Options:\n";
  std::cout << "\n";
  std::cout << "Run modes (mutually exclusive):\n";
  std::cout << "  -f, --singleFile   Test a single file\n";
  std::cout << "  -b, --benchmark    Benchmark test (random values every iteration)\n";
  std::cout << "  -h, --help         Show help message\n";
  std::cout << "\n";
  std::cout << "Common options: \n";
  std::cout << "  -a, --algorithm VAL   0 - Bubble sort\n";
  std::cout << "                        1 - Cocktail sort\n";
  std::cout << "                        2 - Merge sort\n";
  std::cout << "                        3 - Insertion sort\n";
  std::cout << "                        4 - Bucket sort\n";
  std::cout << "                        5 - Quick sort\n";
  std::cout << "                        6 - Shell sort\n";
  std::cout << "  -s, --structure VAL   0 - Array\n";
  std::cout << "                        1 - Single linked list\n";
  std::cout << "                        2 - Double linked list\n";
  std::cout << "                        3 - Queue\n";
  std::cout << "                        4 - Stack\n";
  std::cout << "                        5 - Binary tree\n";
  std::cout << "  -t, --dataType VAL    0 - int\n";
  std::cout << "                        1 - float\n";
  std::cout << "                        2 - double\n";
  std::cout << "                        3 - char\n";
  std::cout << "                        4 - std::string\n";
  std::cout << "                        5 - unsigned int\n";
  std::cout << "                        6 - unsigned long\n";
  std::cout << "                        7 - unsigned char\n";
  std::cout << "  -p, --pivot VAL       0 - random\n";
  std::cout << "                        1 - left\n";
  std::cout << "                        2 - right\n";
  std::cout << "                        3 - middle\n";
  std::cout << "  -e, --shellParameter VAL  0 - option1 (not detailed)\n";
  std::cout << "                            1 - option2 (not detailed)\n";
  std::cout << "                            2 - option3 (not detailed)\n";
  std::cout << "                            3 - option4 (not detailed)\n";
  std::cout << "\n";
  std::cout << "Single file options:\n";
  std::cout << "  -i, --inputFile FILE    File with values to sort\n";
  std::cout << "  -o, --outputFile FILE   Sorted values will be saved  to this file\n";
  std::cout << "\n";
  std::cout << "Benchmark options:\n";
  std::cout << "  -r, --resultsFile FILE   Results (time and parameters) will be saved to this file\n";
  std::cout << "  -d, --distribution VAL   0 - random\n";
  std::cout << "                           1 - ascending\n";
  std::cout << "                           2 - sorted (ascending) in 50%\n";
  std::cout << "                           3 - descending\n";
  std::cout << "  -l, --structureSize LEN  How many elements in the structure.\n";
  std::cout << "  -n, --iterations ITE :)  How many repetitions of the research with the given parameters.\n";
  std::cout << "Examples:\n";
  std::cout << "  ./project --singleFile --inputFile \"in.txt\" --outputFile \"out.txt\" -a 0 -s 1 -t 0\n";
  std::cout << "  Sort values from file \"in.txt\". Bubble sort, single linked list, integers. Save sorted values in \"out.txt\"\n";
  std::cout << "  ./project --benchmark -a 4 -p 1 -s 1 -t 4 -d 3 -l 10000 -n 50 -r \"res.txt\"\n";
  std::cout << "  Repeat following test 50 times: Linked list of 10000 random unsigned int elements initially sorted in descending order. Use quicksort with the left pivot. Results save in \"res.txt\"\n";
  std::cout << "\n";
}

void printParameters()
{
  std::cout << "Parameters values:\n";
  std::cout << LOG_ENUM(runMode) << "\n";
  std::cout << "Common options:\n"
            << LOG_ENUM(algorithm) << ", "
            << LOG_ENUM(structure) << ", "
            << LOG_ENUM(dataType) << ", "
            << LOG_ENUM(pivot) << ", "
            << LOG_ENUM(shellParameter) << "\n";
  std::cout << "Single file options:\n";
  std::cout << LOG(inputFile) << ", "
            << LOG(outputFile) << "\n";
  std::cout << "Benchmark options:\n";
  std::cout << LOG(resultsFile) << ", "
            << LOG_ENUM(distribution) << ", "
            << LOG(structureSize) << ", "
            << LOG(iterations) << "\n";
}

const char *getVersion()
{
  return LIB_VERSION;
}

}
