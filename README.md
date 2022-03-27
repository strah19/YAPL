# YAPL
Yet Another Programming Language

A fast and easy to use programming language created to learn more about language design.

# Development 
The development process is being recorded on <a href = "https://trello.com/b/YdI3P4F4/yapl">Trello</a>. It's there
for your benefit and so you can see the steps that I take and my thought process while building YAPL.

If you are interested in contributing and further develop this language, contact me and look over the YAPL documentation guide
to help you on some basic rules and standards that will keep the code clean, and usable over time.

#Build
To build this project, use CMake. Go into the YAPL directory after it has been cloned, and I suggest creating a build directoyr using `mkdir build`. 
After, go into that directory using `cd build`. Now run `cmake .. -G 'MinGW Makefiles`. This command will create the necessary build files for MinGW 
Makefiles, if you want something else, run `cmake --help` to get a list of supported platforms. After this ha sbeen run, compile the code and run it!
