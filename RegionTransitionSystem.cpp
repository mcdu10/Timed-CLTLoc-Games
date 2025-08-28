//
// Created by Maria Carmen D'urbano on 27/08/25.
//

#include "RegionTransitionSystem.h"

#include <algorithm>



RegionTransition::RegionTransition(const Region& state1, std::string action, const Region& state2):
  source(state1), transition(action), target(state2){}


void RTS::printRTS() {
    std::cout << "=== Regions ===\n";
    std::map<std::string, int> idx;
    for (size_t i = 0; i < regions.size(); ++i) {
        std::cout << "\n";
        std::cout << "R" << i << ": " << "\n";
        regions[i].print();
        idx[regions[i].ID()]=i;
    }

    std::cout << "\n=== Transitions ===\n";
    for (const auto& t : arches) {
        std::cout << "R" << idx[t.source.ID()] << " --(";
        if (t.transition == "tau") {
            std::cout << "delay";
        } else {
            std::cout << t.transition;
        }
        std::cout << ")--> R" << idx[t.target.ID()] << "\n";
    }
}