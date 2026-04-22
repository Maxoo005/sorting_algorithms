## HelloWorld CMake Example

This is an example project showing how to use CMake and build and how to link an external library.

## Project Structure

The project is divided into two parts:
- **1-buildLibrary** - builds a HelloWorld-like library that prints passed arguments. It won't contain ```main()``` function, so it will not work as standalone.
- **2-cmake-useExternalLibrary** - will use a library built above to print passed arguments. This one will have ```main()``` function, so it will create an exectuable.
