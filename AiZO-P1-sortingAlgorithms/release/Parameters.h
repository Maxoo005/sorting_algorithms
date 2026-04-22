#ifndef PARAMETERS_H
#define PARAMETERS_H

#define LIB_VERSION "v.1.2"

#include <string>

namespace Parameters
{
  enum class RunModes {
    undefined = -1,
    singleFile = 0,
    benchmark,
    help
  };

  enum class Algorithms {
    undefined = -1,
    bubble = 0,
    cocktail,
    merge,
    insertion,
    bucket,
    quick,
    shell,

    count
  };

  enum class Structures {
    undefined = -1,
    array = 0,
    singleList,
    doubleList,
    queue,
    stack,
    binaryTree,

    count
  };

  enum class DataTypes {
    undefined = -1,
    typeInt = 0,
    typeFloat,
    typeDouble,
    typeChar,
    typeString,
    tyleUnsignedInt,
    typeUnsignedLong,
    typeUnsignedChar,

    count
  };

  enum class Distribution {
    undefined = -1,
    random = 0,
    ascending,
    ascending50Per,
    descending,

    count
  };

  enum class Pivots {
    undefined = -1,
    random = 0,
    left,
    right,
    middle,

    count
  };

  enum class ShellParameters {
    undefined = -1,
    option1 = 0,
    option2,
    option3,
    option4,

    count
  };

  //
  // Parameters to use.
  //

  extern RunModes runMode;
  extern Algorithms algorithm;
  extern Structures structure;
  extern DataTypes dataType;

  extern std::string inputFile;
  extern std::string outputFile;
  extern std::string resultsFile;

  extern Distribution distribution;

  extern int structureSize;
  extern int iterations;

  extern Pivots pivot;
  extern ShellParameters shellParameter;

  //
  // Methods to use.
  //

  int readParameters(int argc, char **argv);
  void printParameters();

  void enableDebug();
  void disableDebug();

  void help();
  const char *getVersion();
};

#endif
