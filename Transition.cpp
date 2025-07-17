//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#include "Transition.h"

bool Transition::isEnabled(const std::map<std::string, double>& valuation) const {
    return guard.isSatisfiedBy(valuation);
}