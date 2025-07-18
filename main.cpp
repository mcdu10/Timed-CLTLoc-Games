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

    for (auto coppia : r.getRepresentativeValuation()) {
        std::cout << "clock " << coppia.first << " valuation " << coppia.second << "\n";
    };

    std::cout << "\n=== Delay Successors ===\n";
    auto delaySucc = r.delaySuccessors();
    for (size_t i = 0; i < delaySucc.size(); ++i) {
        std::cout << "Successore #" << i+1 << ":\n";
        delaySucc[i].print();
        for (auto coppia : delaySucc[i].getRepresentativeValuation()) {
            std::cout << "clock " << coppia.first << " valuation " << coppia.second << "\n";
        };
        std::cout << "\n";
    }

    std::cout << "\n=== Delay Predecessors ===\n";
    auto delayPred = r.delayPredecessors();
    for (size_t i = 0; i < delayPred.size(); ++i) {
        std::cout << "Predecessore #" << i+1 << ":\n";
        delayPred[i].print();
        for (auto coppia : delayPred[i].getRepresentativeValuation()) {
            std::cout << "clock " << coppia.first << " valuation " << coppia.second << "\n";
        };
        std::cout << "\n";
    }

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
        for (auto coppia : discreteSucc[i].getRepresentativeValuation()) {
            std::cout << "clock " << coppia.first << " valuation " << coppia.second << "\n";
        };
        std::cout << "\n";
    }

    std::cout << "\n=== Discrete Predecessors ===\n";
    auto discretePred = r.discretePredecessors({t, t1});
    for (size_t i = 0; i < discretePred.size(); ++i) {
        std::cout << "Predecessore Discreto #" << i+1 << ":\n";
        discretePred[i].print();
        for (auto coppia : discretePred[i].getRepresentativeValuation()) {
            std::cout << "clock " << coppia.first << " valuation " << coppia.second << "\n";
        };
        std::cout << "\n";
    }

    return 0;
}