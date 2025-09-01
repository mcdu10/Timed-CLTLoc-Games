//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "Region.h"
#include "Transition.h"
#include "image.h"
#include <iostream>
#include <fstream>
#include "TimedArena.h"
#include <filesystem>
#include <iomanip>
#include <random>
#include <locale>
#include "TimedCLTLocGame.h"



namespace fs = std::filesystem;


int main() {
    std::locale::global(std::locale("C"));

    // ----------------------------
    //      ON/OFF  EXAMPLE
    // ----------------------------
    std::map<std::string, double> val;
    val["x"] = 0;
    Region R0(val, "off", 2);
    std::map<std::string, PLAYER> loc = {{"on", PLAYER::controller}, {"off", PLAYER::controller}};

    Guard g;
    Transition switch_on = {"off", "switch_on", g, {"x"}, "on"};
    ClockConstraint clock_constraint = {"x", Comparator::EQ, 2};
    Transition switch_off = {"on", "switch_off", Guard(clock_constraint), {}, "off"};
    std::vector<Transition> transitions = {switch_on, switch_off};

    // ----------------------------
    //       output folder
    // ----------------------------
    fs::path outputDir = std::filesystem::current_path() / "output";
    if (!fs::exists(outputDir)) {
        if (!fs::create_directories(outputDir)) {
            std::cerr << "Error: impossible to create the output folder/\n";
        }
    }

    // Test starting from off, x=0 and going forward
    TAr Arena(loc, R0, transitions);

    // Forward BFS
    auto succFunc = [&transitions](const Region& r) -> RTS {
        return r.successor(transitions);
    };

    std::cout << "\n=== Forward BFS ON/OFF example starting from off, x = 0 ===\n";
    auto start = std::chrono::high_resolution_clock::now();
    RTS systForward = Arena.BFS(succFunc);
    auto end = std::chrono::high_resolution_clock::now();
    systForward.printRTS();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "[Profiling] Forward BFS: " << duration.count() << " ns\n";

    exportRTSGraphSmart(systForward, Arena, (outputDir / "rts_forward.dot").string(), "Forward, example switch on/off");
    openRTSGraphSVG((outputDir / "rts_forward.dot").string(), (outputDir / "rts_forward.svg").string());


    // Backward BFS
    val["x"] = 2;
    Region Rend(val, "off", 2);
    TAr ArenaBack(loc, Rend, transitions);

    auto predFunc = [&transitions](const Region& r) -> RTS {
        return r.predecessor(transitions);
    };

    std::cout << "\n=== Backward BFS ON/OFF example starting from off, x = 2 ===\n";
    start = std::chrono::high_resolution_clock::now();
    RTS systBackward = ArenaBack.BFS(predFunc);
    end = std::chrono::high_resolution_clock::now();
    systBackward.printRTS();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "[Profiling] Backward BFS: " << duration.count() << " ns\n";

    exportRTSGraphSmart(systBackward, ArenaBack, (outputDir / "rts_backward.dot").string(), "Backward, example switch on/off");
    openRTSGraphSVG((outputDir / "rts_backward.dot").string(), (outputDir / "rts_backward.svg").string());

    // Additional test
    val["x"] = 1.3;
    Region Rmix(val, "off", 2);
    TAr ArenaB(loc, Rmix, transitions);

    // both successors and predecessors
    auto neighborsFunc = [&transitions](const Region& r) -> RTS {
        RTS succ = r.successor(transitions);
        RTS pred = r.predecessor(transitions);
        RTS result = succ;
        for (const auto& reg : pred.regions) {
            result.regions.push_back(reg);
        }
        return result;
    };

    std::cout << "\n=== BFS Backward starting from a central region (off, x = 1.3) ===\n";
    start = std::chrono::high_resolution_clock::now();
    RTS systBoth = ArenaB.BFS(predFunc);
    end = std::chrono::high_resolution_clock::now();
    systBoth.printRTS();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "[Profiling] BFS Both: " << duration.count() << " ns\n";

    exportRTSGraphSmart(systBoth, ArenaB, (outputDir / "rts_both.dot").string(), "Backward, example switch on/off");
    openRTSGraphSVG((outputDir / "rts_both.dot").string(), (outputDir / "rts_both.svg").string());

    std::cout << "\n=== BFS Forward starting from a central region (off, x = 1.3) ===\n";
    start = std::chrono::high_resolution_clock::now();
    systBoth = ArenaB.BFS(succFunc);
    end = std::chrono::high_resolution_clock::now();
    systBoth.printRTS();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "[Profiling] BFS Both: " << duration.count() << " ns\n";

    exportRTSGraphSmart(systBoth, ArenaB, (outputDir / "rts_both.dot").string(), "Forward, example switch on/off");
    openRTSGraphSVG((outputDir / "rts_both.dot").string(), (outputDir / "rts_both.svg").string());

    std::cout << "\n=== BFS Forward + Backward starting from a central region (off, x = 1.3) ===\n";
    start = std::chrono::high_resolution_clock::now();
    systBoth = ArenaB.BFS(neighborsFunc);
    end = std::chrono::high_resolution_clock::now();
    systBoth.printRTS();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "[Profiling] BFS Both: " << duration.count() << " ns\n";

    exportRTSGraphSmart(systBoth, ArenaB, (outputDir / "rts_both.dot").string(), "Both Forward and Backward, example switch on/off");
    openRTSGraphSVG((outputDir / "rts_both.dot").string(), (outputDir / "rts_both.svg").string());


    // --------------------------------
    //  Creation of a manual automaton
    // --------------------------------
    std::cout << "\n=== Creation manual automaton ===\n";
    std::vector<std::string> locations = {"loc1", "loc2"};
    std::set<std::string> clocks = {"x","y"};
    Guard g1(ClockConstraint("y", Comparator::EQ, 2));
    Guard g2(ClockConstraint("x", Comparator::GE, 3));

    // Transitions vector
    std::vector<Transition> trans = {
        {"loc1", "a", g1, {"y"}, "loc2"},
        {"loc2", "b", g2, {}, "loc1"}
    };

    std::map<std::string, PLAYER> manualLoc = {{"loc1", PLAYER::controller}, {"loc2", PLAYER::controller}};
    TAr manualAutoma(manualLoc, clocks, trans, 5);
    manualAutoma.print();

    // Lambda for manualAutoma
    auto succFuncManual = [&trans](const Region& r) -> RTS {
        return r.successor(trans);
    };

    start = std::chrono::high_resolution_clock::now();
    RTS manualGraph = manualAutoma.BFS(succFuncManual);
    end = std::chrono::high_resolution_clock::now();
    manualGraph.printRTS();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "[Profiling] Manual automaton BFS: " << duration.count() << " ns\n";

    exportRTSGraphSmart(manualGraph, manualAutoma, (outputDir / "manual.dot").string(), "forward, manual automaton ");
    openRTSGraphSVG((outputDir / "manual.dot").string(), (outputDir / "manual.svg").string());

    // ----------------------------
    //  Parametric profiling
    // ----------------------------
    std::cout << "\n=== Parametric profiling region graph ===\n";
    std::cout << std::left
              << std::setw(8) << "Clocks"
              << std::setw(10) << "Locations"
              << std::setw(13) << "Transitions"
              << std::setw(15) << "Tempo(ns)" << "\n";

    std::vector<int> numClocksList = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    std::vector<int> numLocationsList = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    std::vector<int> numTransitionsList = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};


    // Utility to save results on a file
    auto saveProfilingTable = [&](const std::string& filename, int nClock, int nLoc, int nTrans, double duration_sec) {
        std::ofstream file(outputDir / filename, std::ios::app); // append
        if (!file) {
            std::cerr << "Error: impossible to open " << filename << "\n";
            return;
        }
        file << nClock << ";" << nLoc << ";" << nTrans << ";"
              << duration_sec << "\n";
        file.close();
    };

    // Name file
    std::vector<std::string> profilingFiles = {"profiling_forward.csv", "profiling_backward.csv", "profiling_both.csv"};
    for (auto& f : profilingFiles) {
        std::ofstream file(outputDir / f);
        file << "Clocks;Locations;Transitions;Time(s)\n";
        file.close();
    }

    // --- Forward BFS ---
    std::cout << "\n=== Forward BFS ===\n";
    std::cout << std::left << std::setw(8) << "Clocks"
              << std::setw(10) << "Locations"
              << std::setw(13) << "Transitions"
              << std::setw(15) << "Time(s)" << "\n";

    for (int nClock : numClocksList) {
        for (int nLoc : numLocationsList) {
            for (int nTrans : numTransitionsList) {
                // Construction of the automaton e initialization of the region
                std::vector<std::string> clocksVec;
                for (int i=0;i<nClock;++i) clocksVec.push_back("x"+std::to_string(i));
                std::map<std::string, PLAYER> locationsVec;
                for (int i=0;i<nClock;++i) clocksVec.push_back("x"+std::to_string(i));
                for (int i=0;i<nLoc;++i) locationsVec["loc"+std::to_string(i)] = PLAYER::controller;

                std::vector<Transition> transVec;
                for (int i=0;i<nTrans;++i) {
                    std::string src = "loc" + std::to_string(i % nLoc);
                    std::string dst = "loc" + std::to_string(i % nLoc);
                    transVec.push_back({src,"t"+std::to_string(i),Guard(),{},dst});
                }

                std::map<std::string,double> floorValues;
                for (auto& c : clocksVec) floorValues[c] = 0;
                Region startRegion(floorValues, "loc0", 2);

                TAr arenaParam(locationsVec, startRegion, transVec);

                auto succProf = [&transVec](const Region& r) -> RTS { return r.successor(transVec); };

                auto start = std::chrono::high_resolution_clock::now();
                RTS graphParam = arenaParam.BFS(succProf);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration_sec = end - start;

                // print on console
                std::cout << std::left
                          << std::setw(8) << nClock
                          << std::setw(10) << nLoc
                          << std::setw(13) << nTrans
                          << std::setw(15) << std::scientific << std::setprecision(3) << duration_sec.count() << "\n";

                // Save on file
                saveProfilingTable("profiling_forward.csv", nClock, nLoc, nTrans, duration_sec.count());
            }
        }
    }

    // --- Backward BFS ---
    std::cout << "\n=== Backward BFS ===\n";
    std::cout << std::left << std::setw(8) << "Clocks"
              << std::setw(10) << "Locations"
              << std::setw(13) << "Transitions"
              << std::setw(15) << "Time(s)" << "\n";

    for (int nClock : numClocksList) {
        for (int nLoc : numLocationsList) {
            for (int nTrans : numTransitionsList) {
                std::vector<std::string> clocksVec;
                std::map<std::string, PLAYER> locationsVec;
                for (int i=0;i<nClock;++i) clocksVec.push_back("x"+std::to_string(i));
                for (int i=0;i<nLoc;++i) locationsVec["loc"+std::to_string(i)] = PLAYER::controller;

                std::vector<Transition> transVec;
                for (int i=0;i<nTrans;++i) {
                    std::string src = "loc" + std::to_string(i % nLoc);
                    std::string dst = "loc" + std::to_string(i % nLoc);
                    transVec.push_back({src,"t"+std::to_string(i),Guard(),{},dst});
                }

                std::map<std::string,double> floorValues;
                for (auto& c : clocksVec) floorValues[c] = 0;
                Region startRegion(floorValues, "loc0", 2);

                TAr arenaParam(locationsVec, startRegion, transVec);

                auto prevProf = [&transVec](const Region& r) -> RTS { return r.predecessor(transVec); };

                auto start = std::chrono::high_resolution_clock::now();
                RTS graphParam = arenaParam.BFS(prevProf);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration_sec = end - start;

                std::cout << std::left
                          << std::setw(8) << nClock
                          << std::setw(10) << nLoc
                          << std::setw(13) << nTrans
                          << std::setw(15) << std::scientific << std::setprecision(3) << duration_sec.count() << "\n";

                saveProfilingTable("profiling_backward.csv", nClock, nLoc, nTrans, duration_sec.count());
            }
        }
    }

    // --- Both BFS ---
    std::cout << "\n=== Both Forward + Backward BFS ===\n";
    std::cout << std::left << std::setw(8) << "Clocks"
              << std::setw(10) << "Locations"
              << std::setw(13) << "Transitions"
              << std::setw(15) << "Time(s)" << "\n";

    for (int nClock : numClocksList) {
        for (int nLoc : numLocationsList) {
            for (int nTrans : numTransitionsList) {
                std::vector<std::string> clocksVec;
                for (int i=0;i<nClock;++i) clocksVec.push_back("x"+std::to_string(i));
                std::map<std::string, PLAYER> locationsVec;
                for (int i=0;i<nClock;++i) clocksVec.push_back("x"+std::to_string(i));
                for (int i=0;i<nLoc;++i) locationsVec["loc"+std::to_string(i)] = PLAYER::controller;

                std::vector<Transition> transVec;
                for (int i=0;i<nTrans;++i) {
                    std::string src = "loc" + std::to_string(i % nLoc);
                    std::string dst = "loc" + std::to_string(i % nLoc);
                    transVec.push_back({src,"t"+std::to_string(i),Guard(),{},dst});
                }

                std::map<std::string,double> floorValues;
                for (auto& c : clocksVec) floorValues[c] = 0;
                Region startRegion(floorValues, "loc0", 2);
                TAr arenaParam(locationsVec, startRegion, transVec);

                auto bothProf = [&transVec](const Region& r) -> RTS {
                    RTS succ = r.successor(transVec);
                    RTS pred = r.predecessor(transVec);
                    RTS result = succ;
                    for (const auto& reg : pred.regions) result.regions.push_back(reg);
                    return result;
                };

                auto start = std::chrono::high_resolution_clock::now();
                RTS graphParam = arenaParam.BFS(bothProf);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration_sec = end - start;

                std::cout << std::left
                          << std::setw(8) << nClock
                          << std::setw(10) << nLoc
                          << std::setw(13) << nTrans
                          << std::setw(15) << std::scientific << std::setprecision(3) << duration_sec.count() << "\n";

                saveProfilingTable("profiling_both.csv", nClock, nLoc, nTrans, duration_sec.count());
            }
        }
    }

    std::cout << "Tick period of high_resolution_clock: "
              << (double) std::chrono::high_resolution_clock::period::num
              / std::chrono::high_resolution_clock::period::den
              << " seconds\n";

    std::cout << "\n[Every graph created and profiling tables are in the output folder/]\n";


    // ----------------------------
    //  Testing algorithm1
    // ----------------------------

    // --- 1. Definition of the arena (TAr) ---
    val["x"] = 0;
    R0 = Region(val, "off", 2);
    loc = {{"on", PLAYER::controller}, {"off", PLAYER::controller}};

    Guard guard;
    Transition Switch_on = {"off", "switch_on", g, {"x"}, "on"};
    ClockConstraint Clock_constraint = {"x", Comparator::EQ, 2};
    Transition Switch_off = {"on", "switch_off", Guard(clock_constraint), {}, "off"};
    std::vector<Transition> Transitions = {switch_on, switch_off};

    // --- 2. Winning condition (CLTLocFormula) ---
    Formula phi("off or on and x<1");
    Formula psi("on and x=1");
    // CLTLocFormula
    CLTLocFormula winCond(phi, psi);

    // --- 3. Creation of the TCG ---
    Arena = TAr(loc, R0, Transitions);
    TCG game(Arena, winCond);

    // --- 4. Execution of Algorithm1 ---
    RTS result = game.algorithm1();

    result.printRTS();

    // --- 5. Printing results (example) ---
    exportRTSGraphSmart(result, Arena, (outputDir / "rts_alg.dot").string(), "algorithm1, example switch on/off,\n winning condition: off or on and x<1 Until on and x=1");
    openRTSGraphSVG((outputDir / "rts_alg.dot").string(), (outputDir / "rts_alg.svg").string());


    return 0;
}
