//
// Created by Maria Carmen D'urbano on 15/07/25.
//


#ifndef CLTLOC_FORMULA_H
#define CLTLOC_FORMULA_H

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include "Guard.h"
#include <map>

#include "Region.h"

struct AtomicProposition {
    std::string name;
    AtomicProposition() = default;
    explicit AtomicProposition(const std::string& token);
};

enum class LogicalOperator { AND, OR, NOT };

struct Formula {
    std::vector<std::variant<AtomicProposition, LogicalOperator, ClockConstraint>> operators;


    Formula() = default;
    explicit Formula(const std::string& input) {
        parse(input);
    }

    bool Enabled(std::variant<AtomicProposition, LogicalOperator, ClockConstraint>& op, Region& r);

    void print();

private:
    void parse(const std::string& input);

    static bool isClockConstraint(const std::string& token);

    static ClockConstraint parseClockConstraint(const std::string& token);
};

struct CLTLocFormula {
    Formula phi;
    Formula psi;

    CLTLocFormula(const Formula& p, const Formula& p2) : phi(p), psi(p2) {}
    CLTLocFormula(const std::string& inputphi, const std::string& inputpsi) : phi(inputphi), psi(inputpsi) {}

    bool phiEnabled(Region R);
    bool psiEnabled(Region R);
};



#endif //CLTLOCFORMULA_H
