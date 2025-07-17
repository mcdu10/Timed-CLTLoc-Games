//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "CLTLocFormula.h"
#include <iostream>
#include <cmath>

std::string toString(const Comparator& cmp) {
    switch (cmp) {
    case Comparator::LE: return "≤";
    case Comparator::LT: return "<";
    case Comparator::GE: return "≥";
    case Comparator::GT: return ">";
    case Comparator::EQ: return "=";
    case Comparator::NEQ: return "≠";
    }
    return "?";
}

std::string toString(const ClockTerm& term) {
    if (term.isClock) return term.name;
    return std::to_string(term.constantValue);
}

std::string toString(const CLTLocFormula& formula) {
    if (std::holds_alternative<ClockConstraintFormula>(formula.content)) {
        const auto& c = std::get<ClockConstraintFormula>(formula.content);
        return toString(c.lhs) + " " + toString(c.op) + " " + toString(c.rhs);

    } else if (std::holds_alternative<AtomicProposition>(formula.content)) {
        const auto& p = std::get<AtomicProposition>(formula.content);
        return p.name;

    } else if (std::holds_alternative<LogicalOpFormula>(formula.content)) {
        const auto& op = std::get<LogicalOpFormula>(formula.content);
        switch (op.op) {
        case LogicalOpType::AND:
            return "(" + toString(*op.subformulas[0]) + " ∧ " + toString(*op.subformulas[1]) + ")";
        case LogicalOpType::OR:
            return "(" + toString(*op.subformulas[0]) + " ∨ " + toString(*op.subformulas[1]) + ")";
        case LogicalOpType::NOT:
            return "¬(" + toString(*op.subformulas[0]) + ")";
        case LogicalOpType::NEXT:
            return  "◯(" + toString(*op.subformulas[0]) + ")";
        case LogicalOpType::UNTIL:
            return "(" + toString(*op.subformulas[0]) + " U " + toString(*op.subformulas[1]) + ")";
        case  LogicalOpType::EVENTUALLY:
            return "◊(" + toString(*op.subformulas[0]) + ")";
        }
    }
    return "<?>";
}