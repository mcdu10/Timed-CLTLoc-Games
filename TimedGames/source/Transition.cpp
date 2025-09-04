//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#include "Transition.h"

bool Transition::isEnabled(const std::map<std::string, double>& valuation) const {
    return guard.isSatisfiedBy(valuation);
}

bool Transition::isEnabled(const std::map<std::string, int>& floor,
                          const std::set<std::string>& zero,
                          const std::vector<std::vector<std::string>>& fo) const{
    return guard.isSatisfiedBy(floor, zero, fo);
}

void Transition::print() const{
    std::cout << "Transition: "
               << sourceLocation << " --["
               << action.action;
    if(!guard.constraints.empty()) {
        std::cout << ", ";
        guard.print();
    }
    if (!resetClocks.empty()) {
        std::cout << ", reset {";
        for (auto it = resetClocks.begin(); it != resetClocks.end(); ++it) {
            if (it != resetClocks.begin()) std::cout << ", ";
            std::cout << *it;
        }
        std::cout << "}";
    }
    std::cout << "]--> " << targetLocation << "\n";
}
