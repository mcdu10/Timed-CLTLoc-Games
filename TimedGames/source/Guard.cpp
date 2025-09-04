//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#include "Guard.h"
#include <cmath>
#include <iostream>


ClockConstraint::ClockConstraint(const std::string str, Comparator gt, int i)
    : clock(str), op(gt), constant(i) {}



bool ClockConstraint::evaluate(double value) const {
    switch (op) {
    case Comparator::LE: return value <= constant;
    case Comparator::LT: return value < constant;
    case Comparator::GE: return value >= constant;
    case Comparator::GT: return value > constant;
    case Comparator::EQ: return std::abs(value - constant) < 1e-9;
    case Comparator::NEQ: return std::abs(value - constant) > 1e-9;
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

bool Guard::isSatisfiedBy(const std::map<std::string, int>& floor,
                          const std::set<std::string>& zero,
                          const std::vector<std::vector<std::string>>& fo) const {
    for (const auto& constraint : constraints) {
        const std::string& clk = constraint.clock;
        int f = floor.at(clk);
        bool isZero = (zero.find(clk) != zero.end());
        int c = constraint.constant;

        switch (constraint.op) {
        case Comparator::LE:
            if (isZero) { if (!(f <= c)) return false; }
            else        { if (!(f < c))  return false; }
            break;

        case Comparator::LT:
            if (!(f < c)) return false;
            break;

        case Comparator::GE:
            if (!(f >= c)) return false;
            break;

        case Comparator::GT:
            if (isZero) { if (!(f > c)) return false; }
            else        { if (!(f >= c)) return false; }
            break;

        case Comparator::EQ:
            if (!(isZero && f == c)) return false;
            break;

        case Comparator::NEQ:
            if (isZero && f == c) return false;
            break;
        }
    }
    return true;
}

void Guard::print() const {
    for (auto c: constraints) {
        std::cout << c.clock << " ";
        switch (c.op) {
        case Comparator::LE: std::cout <<  "<="; break;
        case Comparator::LT: std::cout <<  "<"; break;
        case Comparator::GE: std::cout <<  ">="; break;
        case Comparator::GT: std::cout <<  ">"; break;
        case Comparator::EQ: std::cout <<  "="; break;
        case Comparator::NEQ: std::cout <<  "!="; break;
        }
        std::cout << " " << c.constant << std::endl;
    }
}

