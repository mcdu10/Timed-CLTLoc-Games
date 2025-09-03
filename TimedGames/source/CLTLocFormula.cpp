//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "CLTLocFormula.h"
#include <iostream>
#include <cmath>

AtomicProposition::AtomicProposition(const std::string& token)
    : name(token) {}

void Formula::parse(const std::string& input) {
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        if (token == "and") {
            operators.emplace_back(LogicalOperator::AND);
        } else if (token == "or") {
            operators.emplace_back(LogicalOperator::OR);
        } else if (token == "not") {
            operators.emplace_back(LogicalOperator::NOT);
        }
        // clock constraint as "x<=5"
        else if (isClockConstraint(token)) {
            operators.emplace_back(parseClockConstraint(token));
        }
        // otherwise it is an atomic proposition
        else {
            operators.emplace_back(AtomicProposition(token));
        }
    }
};

bool Formula::isClockConstraint(const std::string& token) {
    return token.find('<') != std::string::npos ||
           token.find('>') != std::string::npos ||
           token.find('=') != std::string::npos;
}

ClockConstraint Formula::parseClockConstraint(const std::string& token) {
    std::string clock;
    Comparator op;
    std::string numberStr;

    for (size_t i = 0; i < token.size(); i++) {
        char c = token[i];

        if (std::isdigit(c)) {
            numberStr.push_back(c);
        }
        else if (c == '<' && i + 1 < token.size() && token[i + 1] == '=') {
            op = Comparator::LE;
            i++; // skip '='
        }
        else if (c == '>' && i + 1 < token.size() && token[i + 1] == '=') {
            op = Comparator::GE;
            i++; // skip '='
        }
        else if (c == '<') {
            op = Comparator::LT;
        }
        else if (c == '>') {
            op = Comparator::GT;
        }
        else if (c == '=') {
            op = Comparator::EQ;
        }
        else {
            // assume that is part of the name of the clock
            clock.push_back(c);
        }
    }

    int constant = std::stoi(numberStr);
    return ClockConstraint(clock, op, constant);
}



bool Formula::Enabled(std::variant<AtomicProposition, LogicalOperator, ClockConstraint>& op, Region& r) {
    // ClockConstraint
    if (std::holds_alternative<ClockConstraint>(op)) {
        ClockConstraint c = std::get<ClockConstraint>(op);

        auto itClock = r.floorValues.find(c.clock);
        if (itClock == r.floorValues.end()) return false; // clock not found

        int f = itClock->second;
        bool isZero = (r.zeroFraction.find(c.clock) != r.zeroFraction.end());
        int cnst = c.constant;

        if (cnst > r.maxConstant) return true;
        switch (c.op) {
        case Comparator::LE: return isZero ? (f <= cnst) : (f < cnst);
        case Comparator::LT: return (f < cnst);
        case Comparator::GE: return (f >= cnst);
        case Comparator::GT: return isZero ? (f > cnst) : (f >= cnst);
        case Comparator::EQ: return (isZero && f == cnst);
        case Comparator::NEQ: return !(isZero && f == cnst);
        default: return false;
        }
    }

    // AtomicProposition
    if (std::holds_alternative<AtomicProposition>(op)) {
        AtomicProposition prop = std::get<AtomicProposition>(op);
        return r.location == prop.name;
    }

    // LogicalOperator not evaluable on its own
    return false;
}

void Formula::print() {
    for (auto& op : operators) {
        if(std::holds_alternative<AtomicProposition>(op)) {
            AtomicProposition ap = std::get<AtomicProposition>(op);
            std::cout << "Atomic proposition: " << ap.name << std::endl;
        }
        if(std::holds_alternative<LogicalOperator>(op)) {
            LogicalOperator lop = std::get<LogicalOperator>(op);
            switch (lop) {
            case LogicalOperator::OR: std::cout << "or" << std::endl; break;
            case LogicalOperator::AND: std::cout << "and" << std::endl; break;
            case LogicalOperator::NOT: std::cout << "not" << std::endl; break;
            }
        }
        if(std::holds_alternative<ClockConstraint>(op)) {
            ClockConstraint c = std::get<ClockConstraint>(op);
            std::cout << "Clock constraint: " << c.clock;
            switch (c.op) {
            case Comparator::LE: std::cout << "<=" << c.constant << std::endl; break;
            case Comparator::LT: std::cout << "<" << c.constant << std::endl; break;
            case Comparator::GE: std::cout << ">=" << c.constant << std::endl; break;
            case Comparator::GT: std::cout << ">" << c.constant << std::endl; break;
            case Comparator::EQ: std::cout << "=" << c.constant << std::endl; break;
            case Comparator::NEQ: std::cout << "!=" << c.constant << std::endl; break;
            }
        }
    }
}


bool CLTLocFormula::phiEnabled(Region r) {

    auto it = phi.operators.begin();
    if (it == phi.operators.end()) return false;

    // initial result
    bool result;
    if (!std::holds_alternative<LogicalOperator>(*it))
        result = phi.Enabled(*it, r);
    else {
        ++it;
        result = !phi.Enabled(*it, r);
    }

    ++it;


    while (it != phi.operators.end()) {
        if (std::holds_alternative<LogicalOperator>(*it)) {
            LogicalOperator op = std::get<LogicalOperator>(*it);
            auto nextIt = std::next(it);
            if (nextIt == phi.operators.end()) break;

            bool val;
            if (!std::holds_alternative<LogicalOperator>(*nextIt))
                val = phi.Enabled(*nextIt, r);
            else {
                it = nextIt;
                nextIt = std::next(it);
                val = !phi.Enabled(*nextIt, r);
            }

            if (op == LogicalOperator::AND) {
                result = result && val;
            } else if (op == LogicalOperator::OR) {
                result = result || val;
            }

            it = nextIt; // next element
        }
        ++it;
    }


    return result;
}

bool CLTLocFormula::psiEnabled(Region r) {

    auto it = psi.operators.begin();
    if (it == psi.operators.end()) return false;

    // initial result
    bool result;
    if (!std::holds_alternative<LogicalOperator>(*it))
        result = psi.Enabled(*it, r);
    else {
        ++it;
        result = !psi.Enabled(*it, r);
    }

    ++it;


    while (it != psi.operators.end()) {
        if (std::holds_alternative<LogicalOperator>(*it)) {
            LogicalOperator op = std::get<LogicalOperator>(*it);
            auto nextIt = std::next(it);
            if (nextIt == psi.operators.end()) break;

            bool val;
            if (!std::holds_alternative<LogicalOperator>(*nextIt))
                val = psi.Enabled(*nextIt, r);
            else {
                it = nextIt;
                nextIt = std::next(it);
                val = !psi.Enabled(*nextIt, r);
            }

            if (op == LogicalOperator::AND) {
                result = result && val;
            } else if (op == LogicalOperator::OR) {
                result = result || val;
            }

            it = nextIt; // next element
        }
        ++it;
    }


    return result;
}