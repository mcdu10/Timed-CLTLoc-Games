//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "Region.h"
#include "Transition.h"
#include <iostream>

int main() {
    // valutazione iniziale per i clock
    std::map<std::string, double> valuation = {
        {"x", 1.2},
        {"y", 0.0},
        {"z", 2.9}
    };

    std::string location = "loc0";
    int maxConstant = 5;

    Region r(valuation, location, maxConstant);

    std::cout << "=== Regione iniziale ===\n";
    r.print();

    // rappresentante di r
    std::cout << "=== Representative r ===" << std::endl;
    for (auto coppia : r.getRepresentativeValuation()) {
        std::cout << "clock " << coppia.first << " valuation " << coppia.second << "\n";
    };

    // successori temporali
    std::cout << "\n=== Delay Successors ===\n";
    auto delaySucc = r.delaySuccessor();
        delaySucc.print();
        std::cout << "\n";

    // per i predecessori temporali serve una regione con tutti i clock
    // diversi da zero
    valuation["y"] = 0.5;
    Region r2(valuation, location, maxConstant);
    std::cout << "=== Regione con clock diversi da zero ===\n";
    r2.print();
    std::cout << "\n=== Delay Predecessors ===\n";
    auto delayPred = r2.delayPredecessor();
        delayPred.print();
        std::cout << "\n";

    // per i predecessori/successori discreti servono delle transizioni:
    // una che parta dalla location della regione iniziale e una che arrivi li
    std::string location1 = "loc1";
    std::string action = "a";
    Guard g = {{"x", Comparator::LE, 2.5}}; // x <= 2.5
    Transition t = {
        location,
        action,
        g,
        {"x", "y"},
        location1
    };

    Transition t1 = {
        location1,
        action,
        g,
        {"y"},
        location
    };

    std::cout << "\n=== Discrete Successors ===\n";
    auto discreteSucc = r.discreteSuccessors({t, t1});
    for (size_t i = 0; i < discreteSucc.size(); ++i) {
        std::cout << "Successore Discreto #" << i+1 << ":\n";
        discreteSucc[i].print();
        std::cout << "\n";
    }

    r.print();

    std::cout << "\n=== Discrete Predecessors ===\n";
    auto discretePred = r.discretePredecessors({t, t1});
    for (size_t i = 0; i < discretePred.size(); ++i) {
        std::cout << "Predecessore Discreto #" << i+1 << ":\n";
        discretePred[i].print();
        std::cout << "\n";
    }

    // prova con due clock azzerati
    std::map<std::string, double> val = {
        {"x", 1.2},
        {"y", 0.0},
        {"z", 2.9},
        {"t", 0.0}
    };


    Region r3(val, location, maxConstant);

    std::cout << "=== Regione iniziale ===\n";
    r3.print();

    Transition t2 = {
        location1,
        action,
        g,
        {"y", "t"},
        location
    };

    std::cout << "\n=== Discrete Predecessors ===\n";
    auto dp = r3.discretePredecessors({t, t1, t2});
    for (size_t i = 0; i < dp.size(); ++i) {
        std::cout << "Predecessore Discreto #" << i+1 << ":\n";
        dp[i].print();
        std::cout << "\n";
    }


    return 0;
}