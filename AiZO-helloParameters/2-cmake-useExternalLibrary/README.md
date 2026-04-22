## CMake - linking an external library.

This project is ready-to-go, you can just build it using cmake, but... external, `helloParameters.so` library and its include file `helloParameters.h` are missing.

### Content

- `README.md` - a file you are currently reading.
- `CMakeLists.txt` - a file that tells `cmake` how to build a project, what files should be included, what flags should be set etc.
- `main.cpp` - well... you should know what main.cpp does
- `include` - a directory where external include header files should go
- `lib` - a directory where external libraries should go

### Hot to build

In theory you just need to call `cmake .`, but it creates a bit of a mess in my opinion. It is better to specify a build directory, where the whole mess will be placed.

Commands, as follow **(called in the project root)**:

```
# Initialize project from here (-S .) and decide where build directory should be (-B build).
cmake -S . -B build

# Build an actual executable (compile).
cmake --build build

# Now you can run an executable (it's inside a build directory)!
./useHelloParams <parameter1> <parameter2> ...

# Full clean of building files
rm -rf build

# This is a clean command that will not require initializing everything from scratch.
cmake --build build --target clean
```
