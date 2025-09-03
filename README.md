# Timed-CLTLoc-Games

# Overview

The overview of the folder structure of the project using a **tree structure** is:

```
Timed_CLTLoc_Games/ 
├── cmake-build-debug
├── CMakeLists.txt
├── main.cpp
├── output/
├── README.md
├── TimedGames
│   ├── include
│   │   ├── CLTLocFormula.h
│   │   ├── Guard.h
│   │   ├── Region.h
│   │   ├── RegionTransitionSystem.h
│   │   ├── TimedArena.h
│   │   ├── TimedCLTLocGame.h
│   │   └── Transition.h
│   └── source
│       ├── CLTLocFormula.cpp
│       ├── Guard.cpp
│       ├── Region.cpp
│       ├── RegionTransitionSystem.cpp
│       ├── TimedArena.cpp
│       ├── TimedCLTLocGame.cpp
│       └── Transition.cpp
└── Utility
    ├── include
    │   ├── image.h
    │   └── Validation.h
    └── source
        ├── image.cpp
        └── Validation.cpp
```

The project is organized into two main modules:

- **TimedGames**: This module contains the core classes for managing timed games. The header files are in the `include/` folder and the implementations are in `source/`.
- **Utility**: This module contains supporting functions, such as image handling, and follows the same separation between `include/` and `source/`.

> **Note:** The step-by-step examples explained in this README correspond exactly to the code present in `main.cpp`.

## Required package

The library provides a function for generating a graphical representation of the Region Transition System (RTS) in the form of a Graphviz diagram. To use this functionality, the `Graphviz` package must be installed on the system. On macOS with Homebrew:

brew install graphviz

## Required headers

At the top of your program include the usual headers used by the library and utilities:

#include \<iostream>\
#include \<fstream>\
#include \<filesystem>\
#include \<iomanip>\
#include \<random>\
#include \<locale>\
#include "Utility/include/image.h"\
#include "TimedGames/include/TimedArena.h"

## Constructing a `Region`

A typical constructor used by examples:

std::map<std::string, double> clockValues; \
clockValues["x"] = 0.0;         // x = 0 \
std::string location = "loc"; \
int maxConst = 1;\
Region startRegion(clockValues, location, maxConst);

Alternatively, it can be given the abstract representation:

std::string location = "loc";\
std::map<std::string, int> floorValues = {{"x", 0}};\
std::set\<std::string> zeroFraction = {"x"};\
std::vector<std::vector\<std::string>> fractionalOrder;\
int maxConst = 1;\
Region startRegion(location, floorValues, zeroFraction, fractionalOrder, maxConst);

To check the Region just created:

startRegion.print();

## Defining `guards` and `transitions`

Define atomic clock constraints and compose them into guards:

ClockConstraint cc("x", Comparator::EQ, 1);\
Guard g(cc); // Guard holding the clock constraint

Transition tr = {\
"loc",             // source location\
"a1",              // label\
g,                  // guard\
{"x"},              // resets (clock names)\
"loc1"              // destination location\
};\

To check the Transition just created we can perform a print:\
tr.print();\

## Creating the timed arena (`TAr`)
The two constructors provided are:

std::map<std::string, PLAYER> locMap = { {"loc", PLAYER::controller}, {"loc1", PLAYER::controller} };\
std::vector<Transition> transitions = {tr};\
TAr arena(locMap, startRegion, transitions);

There is a second way to define a TAr:

std::map<std::string, PLAYER> locMap = { {"loc", PLAYER::controller}, {"loc1", PLAYER::controller} };\
std::vector<Transition> transitions = {tr};\
std::set\<std::string> clocks = {"x"};\
TAr arena(locMap, clocks, transitions, maxConst);

in this case, the constructor will create the initial region in the first location and with all clock valuations at $0.0$.

To check the Timed Arena just created we can perform a print:\
arena.print();



## Exploration algorithms (`BFS`)
The library exposes a generic `BFS` method on `TAr`. You pass a neighbor function that returns an `RTS` containing its successor regions (or predecessor regions). Typical neighbor function shapes:

// forward exploration: use region.successor(transitions)\
auto succFunc = [&transitions](const Region& r) -> RTS {\
return r.successor(transitions);\
};

// backward exploration: use region.predecessor(transitions)\
auto predFunc = [&transitions](const Region& r) -> RTS {\
return r.predecessor(transitions);\
};

// combined neighbors (merge successors and predecessors)\
auto bothFunc = [&transitions](const Region& r) -> RTS {\
RTS succ = r.successor(transitions);\
RTS pred = r.predecessor(transitions);\
RTS result = succ;\
for (const auto& reg : pred.regions) result.regions.push_back(reg);\
return result;\
};


Run `BFS` and print the result:

RTS graph = arena.BFS(succFunc);\
graph.printRTS();


## Exporting results
Use the helper functions provided by the library in the folder `Utility/` to export the `RTS` to a DOT file and open an SVG:

exportRTSGraphSmart(graph, arena, "output/rts_forward.dot", "Forward exploration");\
openRTSGraphSVG("output/rts_forward.dot", "output/rts_forward.svg");

It is recommended to previously create a folder (where `namespace fs = std::filesystem;`):

fs::path outputDir = std::filesystem::current_path() / "output";\
if (!fs::exists(outputDir)) {\
if (!fs::create_directories(outputDir)) {\
std::cerr << "Error: impossible to create the output folder/\n";\
}
}

After executing the commands above, the `.svg` file should automatically open with the system’s default application, and the files are stored in the `output` folder. 