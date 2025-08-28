//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#ifndef TIMEDARENA_H
#define TIMEDARENA_H

#include "Guard.h"
#include "Transition.h"
#include "CLTLocFormula.h"
#include "Region.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <variant>
#include <memory>
#include <iostream>
#include <algorithm>
#include <functional>
#include "RegionTransitionSystem.h"

namespace TimedArena {

    struct Clock {
        std::string name;
    };

    using ClockValuation = std::map<std::string, double>;
    using Condition = CLTLocFormula;


    class TAr {
    public:
        Region R;
        std::vector<Transition> transitions;

        TAr(const Region& R, const std::vector<Transition>& transitions);
        TAr(const std::vector<std::string>& locations,
     const std::set<std::string>& clocks,
     const std::vector<Transition>& trans,
     const int max);


        RTS BFS(std::function<RTS(const Region&)> neighborFunc);

        bool reachable(const Region& r);

        void print();
    };


}

#endif // TIMEDARENA_H

