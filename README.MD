# Vehicles in C++

This is partly based on my existing project [Coding Challenge #69 Redone - Evolving Steering Behaviors](https://github.com/elunico/CC-069-Redone) which itself was based on a Coding Train Coding Challenge. 

I wanted to reimplement this in C++ to mostly use C++ in a more practical because I haven't been keeping up with my C++ skills lately. I also wanted a chance to explore some GUI programming in C++. 

## Requirements 

You need to have CMAKE, make, ncurses and FLTK to run this program currently. 

Build with 

```sh
cmake .
``` 

in the root folder than `make -j` and then you can run `./main`

## Running and Issues

When running the program has a 1800x1000 screen by default. This can be changed. You can switch to a more rudimentary Curses-based renderer in the terminal, but you should adjust the screen size before doing so. Furthermore, you will still need both ncurses and FLTK to build the project. 

There is a quadtree implementation for this program. The qt is constructed on the stack (although the data is in a vector on the heap) every frame. It seems due to the clustering of the vehicles (which is expected behavior based on how their programmed) the construction of the quadtree greater than the relief it provides in checking and it appears to slow down the program especially when the number of vehicles gets very high and the spread is dense. 

There are also other issues with the quadtree. For example, the same seed consistently produces the same results when quadtree is disabled, but different results when quadtree is enabled. Clearly, something in the implementation of the quadtree is affecting the outcome of the pseudorandom events. 

Almost all the classes in this program are simple types with no special member functions, no templates, no SFINAE, etc. They are mostly a direct port of the JavaScript code from the original challenge. It is likely that much optimization can take place, although it is worth noting that the JS version slows to a crawl around 1000 vehicles whereas this takes until about 6000 for that to happen. 

## Controls 

When the program is running with the FLTK renderer (which is default), you can click on a vehicle to highlight it. Doing so will render the vehicle in blue and render its perception radius in green. It will also highlight all the vehicles that it sees as its "neighbors" in red. When using the quadtree only some nearby vehicles will be red, but without the quadtree it must consider every vehicle its neighbor to be checked. You can highlight multiple vehicles at once and press `c` to clear all highlights

You can be press the space bar to pause the simulation but allow the graphics to still update. Continue pressing SPACE to advance 1 tick at a time. Press `s` to resume the simulation. 

Pressing `q` will enable and disable the building and use of the quadtree in determining a vehicles neighbors. You can see the current status by looking for `[PLAIN]` or `[QT ON]` in the top status line. 

Closing the window will end the simulation and print a summary. 

## Demonstration
![Demo Gif](https://app.eluni.co/vehicle-demo)
