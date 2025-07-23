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

    ClockValuation initialValuation;
    Condition winningCondition;

    // Discrete transition predecessor
    std::vector<TransitionRelation> Omega (TAr A, std::vector<State> G);

    // Delay transition predecessor (controller e envoirement)
    std::vector<TransitionRelation> Deltac (TAr A, std::vector<State> G);
    std::vector<TransitionRelation> Deltae (TAr A, std::vector<State> G);

    // Overall transition predecessor
    std::vector<TransitionRelation> Theta (TAr A, std::vector<State> G);

    // Controllable predecessor
    std::vector<State> Pi (TAr A, std::vector<State> G);

    void algorithm1();
};

#endif //TIMEDGAME_H
