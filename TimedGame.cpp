//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "TimedGame.h"


std::vector<RegionTransition> TimedGame::Omega (TAr A, std::set<State> G) {
    std::vector<RegionTransition> result;
    for (auto a : A.actions) {
        bool rej = true;
        std::vector<RegionTransition> tmp;
        for (auto tr : A.transitions) {
            if (tr.action != a) continue;
            for (auto s : G) {
                if (tr.targetLocation == s.region.location) {
                    rej = false;
                    break;
                }
            }
            if (rej) break;
            tmp.push_back(RegionTransition(Region(initialValuation, tr.sourceLocation, max), a, Region(initialValuation, tr.targetLocation, max)));
        }
        if (!rej) {
            result.insert(result.end(), tmp.begin(), tmp.end());
        }

    }
    return result;
}

std::vector<RegionTransition> TimedGame::Deltac (TAr A, std::set<State> G) {
    std::vector<RegionTransition> result;

};
