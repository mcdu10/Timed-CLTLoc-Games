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

namespace TimedArena {

    struct Clock {
        std::string name;
    };

    using ClockValuation = std::map<std::string, double>;
    using Condition = CLTLocFormula;

    struct RegionTransition {
        Region source;
        std::variant<Act, double> transition;
        Region target;

        RegionTransition();
        RegionTransition(const Region& state1, double time, const Region& state2);
        RegionTransition(const Region& state1, Act action, const Region& state2);

    };

    struct RTS {
        std::vector<Region> regions;
        std::vector<RegionTransition> arches;

        void printRTS();
    };

    class TAr {
    public:
        Region R;
        std::vector<Transition> transitions;

        TAr(const Region& R, const std::vector<Transition>& transitions);

        RTS BFS(std::function<std::vector<Region>(const Region&)> neighborFunc);

        bool reachable(const Region& r);
    };


}

#endif // TIMEDARENA_H

