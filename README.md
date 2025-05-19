Welcome to *tSnakes*, an offline Splix.io remake with bots!
------------------------------------------------------------

Requirements for compilation:
- build environment: GCC/gpp + CMake
- SFML (Simple Fast Media Library) developer libraries
- arial.ttf (included under 'assets')


To build it, do these steps:
- unzip to a folder (eg. "tSnakes")
- in terminal "CD tSnakes"
- create this directory/folder inside it: "build"
- copy "assets" folder into that "build"-folder (so that arial.ttf can be found by the compiler)
- "CD build"
- "cmake .."  to create the makefiles  (release version is selected by default, using all viable optimizations)
- "cmake --build ."   (or "cmake --build . --clean-first" if you want to recompile it)

Now you can invoke it with parameters: "./tSnakes [parameters]"
 " --help" or "-h"  Shows the help message (listing all possible parameters)
Possible parameters: number of bots, size of arena(X/Y), player speed and bot speed

Keys in game:
- [cursor] keys to move your snake (standard Splix rules apply)
- 'P' to pause your snake movement
- [Esc] to quit and save the game

At start, the game tries to load a saved game.

(Known issue: If you saved a game with non-default arena-size, you need to give the same size-parameters at start to be able to load it.)


Happy snake hunting! :)


--------------------------------------------------------------

![tSnakes](https://github.com/JSettler/tSnakes/blob/master/tSnakes.png)


