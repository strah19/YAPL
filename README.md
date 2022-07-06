# YAPL
Yet Another Programming Language

A fast and easy to use programming language created to learn more about language design.

# Development 
If you are interested in contributing and further develop this language, contact me and look over the YAPL documentation guide
to help you on some basic rules and standards that will keep the code clean, and usable over time.

# Build
To build this project, use CMake. Go into the YAPL directory after it has been cloned, and I suggest creating a build directoyr using `mkdir build`. 
After, go into that directory using `cd build`. Now run `cmake .. -G "MinGW Makefiles"`. This command will create the necessary build files for MinGW 
Makefiles, if you want something else, run `cmake --help` to get a list of supported platforms. After this has been done, compile the code and run it!
You can run YAPL from the build directory or create an install directory. Go back to the main directory and run `mkdir install`. Then run 
`cmake --install . --prefix "install"`. Now go into `install/bin` to run YAPL. If you would like to run the YAPL tests, go back into the build folder
and run `ctest`.