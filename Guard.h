//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#ifndef GUARD_H
#define GUARD_H

#include <vector>
#include <sstream>
#include <map>
#include <set>


enum class Comparator { LE, LT, GE, GT, EQ, NEQ };

struct ClockConstraint {
    std::string clock;
    Comparator op;
    int constant;

    bool evaluate(double value) const;
    ClockConstraint() = default;
    ClockConstraint(const char* str, Comparator gt, int i);
};

class Guard {
public:
    std::vector<ClockConstraint> constraints;

    Guard() = default;
    Guard(std::vector<ClockConstraint> list) : constraints(list) {};
    Guard(const ClockConstraint& c) {
        constraints.push_back(c);
    };

    bool isSatisfiedBy(const std::map<std::string, double>& valuation) const;
    bool isSatisfiedBy( const std::map<std::string, int>& floor, const std::set<std::string>& zero, const std::vector<std::vector<std::string>>& fo) const;
};
#endif //GUARD_H
