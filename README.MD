# Vehicles in C++

This is partly based on my existing project [Coding Challenge #69 Redone - Evolving Steering Behaviors](https://github.com/elunico/CC-069-Redone) which itself was based on a Coding Train Coding Challenge.

I wanted to reimplement this in C++ to mostly use C++ in a more practical because I haven't been keeping up with my C++ skills lately. I also wanted a chance to explore some GUI programming in C++.

Even though this is based on that challenge, there are significant behavioral differences in this implementation mostly down to better ways to optimize and clearer code which lead to more realistic decisions than what was present in the JS version. The most notable of the many changes is that once a vehicle or food is close enough to be sought, it remains the seeking target until it is out of sight, whereupon a new target can be found and sought. Additionally, the vehicles seek only food below a certain health rather than food and other vehicles. 

## Requirements

You need to have CMAKE and make to build this. The GUI is written in FLTK which is required for a GUI rendering. 

### Building 
Build with

```sh
cmake -DRELEASE_BUILD=yes . # RELEASE_BUILD turns on -O3 optimization and disables sanitizers. 
make -j
./main # run the executable
```

### Without FLTK

You can choose to build 
in non-GUI mode which will print a world summery to the Console but run the simulation normally behind the scenes, but no visualization is present. In this mode, FLTK is not used and it is not required for building. 

To do this you can use the `-DNOGUI=yes` flag on the cmake build command. 

```sh
cmake -DRELEASE_BUILD=yes -DNOGUI=yes .
```

## Controls

When the program is running with the FLTK renderer (which is default), you can click on a vehicle to highlight it. You will also see a control window with several buttons which will help you control the simulation.

When using the NOGUI mode without FLTK, the program will respond to a `SIGINT` by pausing the simulation and presenting the user with a TUI for options to modify the parameters of the simulation. 

## Demonstration
[Demo Gif](https://app.eluni.co/vehicle-demo)
