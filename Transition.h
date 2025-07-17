//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#ifndef TRANSITION_H
#define TRANSITION_H

#include "Guard.h"
#include <string>
#include <set>

struct Transition {
    std::string sourceLocation;
    std::string action;
    Guard guard;
    std::set<std::string> resetClocks;
    std::string targetLocation;

    // controllo guardie
    bool isEnabled(const std::map<std::string, double>& valuation) const;
};

#endif //TRANSITION_H
