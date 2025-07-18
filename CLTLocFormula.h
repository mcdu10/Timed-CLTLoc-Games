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

struct ClockTerm {
    bool isClock;
    std::string name;
    double constantValue = 0.0;
};

struct ClockConstraintFormula {
    ClockTerm lhs;
    Comparator op;
    ClockTerm rhs;
};

struct AtomicProposition {
    std::string name;
};

enum class LogicalOpType { AND, OR, NOT, NEXT, UNTIL, EVENTUALLY };

struct LogicalOpFormula;

struct CLTLocFormula;

using FormulaContent = std::variant<ClockConstraintFormula, AtomicProposition, LogicalOpFormula>;

struct LogicalOpFormula {
    LogicalOpType op;
    std::vector<std::shared_ptr<CLTLocFormula>> subformulas;
};

struct CLTLocFormula {
    FormulaContent content;

    CLTLocFormula(const ClockConstraintFormula& f) : content(f) {}
    CLTLocFormula(const AtomicProposition& p) : content(p) {}
    CLTLocFormula(const LogicalOpFormula& op) : content(op) {}
};

std::string toString(const Comparator& cmp);
std::string toString(const ClockTerm& term);
std::string toString(const CLTLocFormula& formula);


#endif //CLTLOCFORMULA_H
