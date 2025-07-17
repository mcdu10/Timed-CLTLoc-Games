//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#ifndef GUARD_H
#define GUARD_H

#include <vector>
#include <sstream>
#include <map>

enum class Comparator { LE, LT, GE, GT, EQ };

struct ClockConstraint {
    std::string clock;
    Comparator op;
    double constant;

    bool evaluate(double value) const;
};

class Guard {
public:
    std::vector<ClockConstraint> constraints;

    Guard() = default;
    Guard(std::initializer_list<ClockConstraint> list) : constraints(list) {}

    bool isSatisfiedBy(const std::map<std::string, double>& valuation) const;
};
#endif //GUARD_H
