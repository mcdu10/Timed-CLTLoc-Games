//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#include "Guard.h"
#include <cmath>

bool ClockConstraint::evaluate(double value) const {
    switch (op) {
    case Comparator::LE: return value <= constant;
    case Comparator::LT: return value < constant;
    case Comparator::GE: return value >= constant;
    case Comparator::GT: return value > constant;
    case Comparator::EQ: return std::abs(value - constant) < 1e-9;
    }
    return false;
}

bool Guard::isSatisfiedBy(const std::map<std::string, double>& valuation) const {
    for (const auto& constraint : constraints) {
        auto it = valuation.find(constraint.clock);
        if (it == valuation.end()) return false;
        if (!constraint.evaluate(it->second)) return false;
    }
    return true;
}