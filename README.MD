# Vehicles in C++

This is partly based on my existing project [Coding Challenge #69 Redone - Evolving Steering Behaviors](https://github.com/elunico/CC-069-Redone) which itself was based on a Coding Train Coding Challenge.

I wanted to reimplement this in C++ to mostly use C++ in a more practical because I haven't been keeping up with my C++ skills lately. I also wanted a chance to explore some GUI programming in C++.

Even though this is based on that challenge, there are significant behavioral differences in this implementation mostly down to better ways to optimize and clearer code which lead to more realistic decisions than what was present in the JS version. The most notable of the many changes is that once a vehicle or food is close enough to be sought, it remains the seeking target until it is out of sight, whereupon a new target can be found and sought. Additionally, the vehicles seek only food below a certain health rather than food and other vehicles. 

## Requirements

You need to have CMAKE, make, and FLTK to run this program currently.

Build with

```sh
cmake -DRELEASE_BUILD=yes . # RELEASE_BUILD turns on -O3 optimization and disables sanitizers. 
make -j
./main # run the executable
```

## Controls

When the program is running with the FLTK renderer (which is default), you can click on a vehicle to highlight it. You will also see a control window with several buttons which will help you control the simulation.

Currently there are no ways to control the simulation in curses mode.

## Demonstration
[Demo Gif](https://app.eluni.co/vehicle-demo)
