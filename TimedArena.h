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
#include <memory>
#include <iostream>

namespace TimedArena {

    enum class Player { Controller, Environment };

    struct action {
        std::string act;
    };

    struct Clock {
        std::string name;
    };

    using ClockValuation = std::map<std::string, double>;
    using Condition = CLTLocFormula;

    struct State {
        std::string location;
        Region region;
        Player player ;

        void print() const {
            if (player ==  Player::Controller)
             std::cout << "Controller " << std::endl;
            else std::cout << "Environment " << std::endl;
            std::cout << "location: " << location;
            region.print();
        }
    };

    class TAr {
    public:
        std::vector<action> actions;
        std::vector<std::string> locations;
        std::string initialLocation;
        std::vector<Clock> clocks;
        std::vector<double> initialValuation;
        std::vector<Transition> transitions;

        void buildRegionTransitionSystem();

    };


}

#endif // TIMEDARENA_H

