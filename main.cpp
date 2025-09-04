//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <random>
#include <locale>
#include "Utility/include/image.h"
#include "TimedGames/include/TimedArena.h"
#include "Utility/include/Validation.h"
#include "Utility/include/TCGexample.h"



namespace fs = std::filesystem;


int main() {
    // run Validation tests for the library and BFS algorithm
    // runValidation();

    // run example for algorithm1
    // runVal();


    // CREATE REGION
    std::map<std::string, double> clockValues;
    clockValues["x"] = 0.0;         // x = 0
    std::string location = "loc";
    int maxConst = 1;
    Region startRegion(clockValues, location, maxConst);
    // print region
    startRegion.print();

    // CREATE TRANSITION
    ClockConstraint cc("x", Comparator::EQ, 1);
    Guard g(cc); // Guard holding the clock constraint
    Transition tr = {
        "loc",             // source location
        "a1",               // label
        g,                  // guard
        {"x"},              // resets (clock names)
        "loc1"              // destination location
    };
    // print transition
    tr.print();

    // CREATE TIMED ARENA
    std::map<std::string, PLAYER> locMap = { {"loc", PLAYER::controller}, {"loc1", PLAYER::controller} };
    std::vector<Transition> transitions = {tr};
    TAr arena(locMap, startRegion, transitions);
    // print arena
    arena.print();

    // CREATE LAMBDA FUNCTION
    // forward exploration: use region.successor(transitions)
    auto succFunc = [&transitions](const Region& r) -> RTS {
        return r.successor(transitions);
    };
    // backward exploration: use region.predecessor(transitions)
    auto predFunc = [&transitions](const Region& r) -> RTS {
        return r.predecessor(transitions);
    };
    // combined neighbors (merge successors and predecessors)
    auto bothFunc = [&transitions](const Region& r) -> RTS {
        RTS succ = r.successor(transitions);
        RTS pred = r.predecessor(transitions);
        RTS result = succ;
        for (const auto& reg : pred.regions) result.regions.push_back(reg);
        return result;
    };

    // BFS ALGORITHM
    RTS graph = arena.BFS(succFunc);
    // print RTS
    graph.printRTS();

    // EXPORT RESULTS
    //create folder
    fs::path outputDir = std::filesystem::current_path() / "output";
    if (!fs::exists(outputDir)) {
        if (!fs::create_directories(outputDir)) {
            std::cerr << "Error: impossible to create the output folder/\n";
        }
    }
    // export
    exportRTSGraphSmart(graph, arena, "output/rts_forward.dot", "Forward exploration");
    openRTSGraphSVG("output/rts_forward.dot", "output/rts_forward.svg");



    return 0;
}
