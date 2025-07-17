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

    void algorithm1();
};

#endif //TIMEDGAME_H
