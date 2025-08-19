//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#ifndef TIMEDGAME_H
#define TIMEDGAME_H

#include "TimedArena.h"
#include "Guard.h"
#include "Transition.h"
#include "CLTLocFormula.h"
#include "Region.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>

using namespace TimedArena;

class TimedGame
{
public:
    TAr A;

    int max;

    std::set<State> ContrStates;
    std::set<State> EnvStates;

    ClockValuation initialValuation;
    Condition winningCondition;


    // Discrete transition predecessor
    std::vector<RegionTransition> Omega (TAr A, std::set<State> G);

    // Delay transition predecessor (controller e envoirement)
    std::vector<RegionTransition> Deltac (TAr A, std::set<State> G);
    std::vector<RegionTransition> Deltae (TAr A, std::set<State> G);

    // Overall transition predecessor
    std::vector<RegionTransition> Theta (TAr A, std::set<State> G);

    // Controllable predecessor
    std::vector<State> Pi (TAr A, std::set<State> G);

    void algorithm1();
};

#endif //TIMEDGAME_H
