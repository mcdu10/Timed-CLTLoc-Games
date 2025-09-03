//
// Created by Maria Carmen D'urbano on 03/09/25.
//

#include "TCGexample.h"

#include <sstream>
#include <map>
#include <filesystem>
#include <random>
#include <iostream>

#include "Guard.h"
#include "image.h"
#include "TimedArena.h"
#include "Transition.h"
#include "TimedCLTLocGame.h"

namespace fs = std::filesystem;

void runVal() {

    std::vector<Transition> transitions;

    // s0 -> s1 : x >= 2, reset
    transitions.push_back({
        "sO",
        Act{"go"},
        Guard({ ClockConstraint("x", Comparator::GE, 2) }),
        {"x"},
        "sI"
    });

    // s1 -> s0 : x >= 1, reset
    transitions.push_back({
        "sI",
        Act{"back"},
        Guard({ ClockConstraint("x", Comparator::GE, 1) }),
        {"x"},
        "sO"
    });

    // giocatori (controller everywhere per semplicità)
    std::map<std::string, PLAYER> locations;
    locations["sO"] = PLAYER::environment;
    locations["sI"] = PLAYER::controller;

    // stato iniziale
    std::map<std::string, double> val = {{"x", 0.0}, {"y", 0.0}};
    Region R0(val, "sO", 2);

    std::cout << "val" << std::endl;

    // winning condition
    Formula phi{"x>=0"};
    Formula psi{"sO and y>=3 and y<=4"};
    CLTLocFormula winCond(phi, psi);


    TAr arena(locations, R0, transitions);

    TCG game(arena, winCond);

    RTS result = game.algorithm1();

    result.printRTS();

    fs::path outputDir = std::filesystem::current_path() /".."/ "output";
    if (!fs::exists(outputDir)) {
        if (!fs::create_directories(outputDir)) {
            std::cerr << "Error: impossible to create the output folder/\n";
        }
    }

    exportRTSGraphSmart(result, arena, (outputDir / "rts_alg.dot").string(), "algorithm1, Production Cell,\n winning condition: ¬fall_d ∧ ¬fall_p ∧ y > 0) U (depot ∧ 25 ≤ y ≤ 3");
    openRTSGraphSVG((outputDir / "rts_alg.dot").string(), (outputDir / "rts_alg.svg").string());





}
