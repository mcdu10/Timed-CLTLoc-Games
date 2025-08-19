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

namespace TimedArena {

    enum class Player { Controller, Environment };

    struct Clock {
        std::string name;
    };

    using ClockValuation = std::map<std::string, double>;
    using Condition = CLTLocFormula;

    struct State {
        Region region;

        State(const Region& region);

        void print() const;

    };

    struct RegionTransition {
        State source;
        std::variant<Act, double> transition;
        State target;

        RegionTransition();
        RegionTransition(const State& state1, double time, const State& state2);
        RegionTransition(const State& state1, Act action, const State& state2);

    };

    class TAr {
    public:
        std::vector<Act> actions;
        std::vector<std::string> locations;
        std::string initialLocation;
        std::vector<Clock> clocks;
        std::vector<Transition> transitions;

        TAr(const std::vector<Act>& actions, const std::vector<std::string>& locations, const std::string& initialLocation, const std::vector<Clock>& clocks, const std::vector<Transition>& transitions);

    };


}

#endif // TIMEDARENA_H

