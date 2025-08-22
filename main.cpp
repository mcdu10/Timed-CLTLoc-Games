//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "Region.h"
#include "Transition.h"
#include <iostream>

#include "TimedArena.h"

int main() {
    std::map<std::string, double> val;
    val["x"] = 0;
    Region R0(val, "off", 2);

    Guard g;
    Transition switch_on = {
        "off",
        "switch_on",
        g,
        {"x"},
        "on"
    };

    ClockConstraint clock_constraint = {
        "x",
        Comparator::EQ,
        2
    };

    Transition switch_off = {
        "on",
        "switch_off",
        Guard(clock_constraint),
        {},
        "off"
    };

    std::vector<Transition> transitions = {switch_on, switch_off};

    TimedArena::TAr Arena(R0, transitions);

    auto neighborFunc = [&transitions](const Region& r) -> std::vector<Region> {
        return r.successor(transitions);
    };

    std::cout << "Partendo dalla posizione: off, x = 0 si costruiscono le regioni successive" << std::endl;

    TimedArena::RTS syst = Arena.BFS(neighborFunc);
    syst.printRTS();


    val["x"] = 2;
    Region Rend(val, "off", 2);
    TimedArena::TAr Arena1(Rend, transitions);
    auto neighborFunc1 = [&transitions](const Region& r) -> std::vector<Region> {
        return r.predecessor(transitions);
    };

    std::cout << "Partendo dalla posizione: off, x = 2 si costruiscono le regioni precedenti" << std::endl;

    TimedArena::RTS syst1 = Arena1.BFS(neighborFunc1);
    syst1.printRTS();

    val["x"] = 2;
    Region X(val, "on", 2);
    std::cout << Arena1.reachable(X) << std::endl;

    return 0;
}
