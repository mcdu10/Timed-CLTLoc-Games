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
        std::string location;
        Region region;
        Player player ;

        State(const Region& region, Player player);

        void print() const {
            if (player ==  Player::Controller)
             std::cout << "Controller " << std::endl;
            else std::cout << "Environment " << std::endl;
            std::cout << "location: " << region.location;
            region.print();
        }

    };

    struct TransitionRelation {
        State initial;
        std::variant<Act, double> transition;
        State ending;

        TransitionRelation(const State& state, double second, const State& ending);
    };

    class TAr {
    public:
        std::vector<Act> actions;
        std::vector<std::string> locations;
        std::string initialLocation;
        std::vector<Clock> clocks;
        std::vector<Transition> transitions;

    };


}

#endif // TIMEDARENA_H

