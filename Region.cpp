//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#include "Region.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <optional>
#include <variant>
#include <_strings.h>
#include "RegionTransitionSystem.h"



// Costruttore
Region::Region(const std::map<std::string, double>& valuation, const std::string& loc, int maxConst)
    : location(loc), maxConstant(maxConst)
{
    std::map<std::string, double> fracPart;

    for (const auto& [clock, value] : valuation) {
        if (value > maxConstant) continue;

        int floorVal = static_cast<int>(value);
        floorValues[clock] = floorVal;

        double frac = value - floorVal;
        if (std::abs(frac) < 1e-9)
            zeroFraction.insert(clock);
        else
            fracPart[clock] = frac;
    }

    // Ordine crescente della parte decimale
    while (!fracPart.empty()) {
        double minFrac = std::min_element(fracPart.begin(), fracPart.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; })->second;

        std::vector<std::string> group;
        for (auto it = fracPart.begin(); it != fracPart.end(); ) {
            if (std::abs(it->second - minFrac) < 1e-6) {
                group.push_back(it->first);
                it = fracPart.erase(it);
            } else {
                ++it;
            }
        }
        fractionalOrder.push_back(group);
    }
}

Region::Region(const std::string& loc, const std::map<std::string, int>& floor, const std::set<std::string>& zero, const std::vector<std::vector<std::string>>& fo, int max_constant):
 location(loc), floorValues(floor), zeroFraction(zero), fractionalOrder(fo), maxConstant(max_constant){}

Region::Region() : maxConstant(0), location("loc0") {} // costruttore di default



bool Region::isEquivalentTo(const Region& other) const {
    if (floorValues != other.floorValues) return false;
    if (zeroFraction != other.zeroFraction) return false;
    if (fractionalOrder.size() != other.fractionalOrder.size()) return false;

    for (size_t i = 0; i < fractionalOrder.size(); ++i) {
        std::set<std::string> a(fractionalOrder[i].begin(), fractionalOrder[i].end());
        std::set<std::string> b(other.fractionalOrder[i].begin(), other.fractionalOrder[i].end());
        if (a != b) return false;
    }
    return true;
}

bool Region::containsRegionEquivalentTo(const std::vector<Region>& vec, const Region& target) {
    for (const auto& r : vec) {
        if (r.isEquivalentTo(target)) return true;
    }
    return false;
}


std::map<std::string, double> Region::getRepresentativeValuation()const{
    std::map<std::string, double> values;

    // Inizializza i valori con la parte intera
    for (const auto& [clock, floor] : floorValues) {
        values[clock] = static_cast<double>(floor);
    }

    // Aggiungi frazioni a chi non è in zeroFraction rispettando l'ordine crescente
    double fracValue = 0.1;
    for (const auto& group : fractionalOrder) {
        for (const auto& clk : group) {
            if (zeroFraction.find(clk) == zeroFraction.end()) {
                values[clk] += fracValue;
            }
        }
        fracValue += 0.1;
    }

    return values;
};


std::optional<Region> Region::delaySuccessor() const {
    std::vector<std::vector<std::string>> fo;
    std::set<std::string> zero;

    if (!(std::max_element(
                   floorValues.begin(), floorValues.end(),
                   [](const auto& a, const auto& b) {
                       return a.second < b.second;
                   })->second < maxConstant))
        return std::nullopt;

    if (!zeroFraction.empty()) {
        fo = fractionalOrder;
        fo.insert(fo.begin(), std::vector<std::string>(zeroFraction.begin(), zeroFraction.end()));
        return Region(location, floorValues, zero, fo, maxConstant);
    }

    std::vector<std::string> greater = fractionalOrder.back();
    zero.insert(greater.begin(), greater.end());
    fo = std::vector<std::vector<std::string>>(fractionalOrder.begin(), fractionalOrder.end()-1);

    std::map<std::string, int> floor = floorValues;
    bool outofrange = false;
    for (auto it : zero) {
        if (floor[it]+1 > maxConstant) {
            outofrange = true;
            break;
        }
        floor[it] = floor[it] + 1;
    }

    if (outofrange) {
        return Region(location, floorValues, zeroFraction, fractionalOrder, maxConstant);
    }

    return Region(location, floor, zero, fo, maxConstant);
}


std::optional<Region> Region::delayPredecessor() const {

    auto minclk = std::min_element(
                   floorValues.begin(), floorValues.end(),
                   [](const auto& a, const auto& b) {
                       return a.second < b.second;
                   });


    if (minclk->second == 0 && zeroFraction.find(minclk->first) != zeroFraction.end()) {
        return std::nullopt;
    }

    std::set<std::string> zero;
    std::vector<std::vector<std::string>> fo = fractionalOrder;
    std::map<std::string, int> floor = floorValues;
    if (!zeroFraction.empty()) {
        std::vector<std::string> lower = std::vector<std::string>(zeroFraction.begin(), zeroFraction.end());
        fo.push_back(lower);
        for (auto it : zeroFraction) {
            floor[it] = floor[it] - 1;
        }
        return Region(location, floor, zero, fo, maxConstant);
    }

    for (auto it : fractionalOrder.front()) {
        zero.insert(it);
    }
    fo.erase(fo.begin());
    return Region(location, floor, zero, fo, maxConstant);
}


RTS Region::discreteSuccessors(const std::vector<Transition>& transitions) const {
    RTS result;
    result.regions.push_back(*this);

    for (const auto tr : transitions) {
        // controlla che la transizione parta dalla location corrente e che le
        // relative guardie siano soddisfatte
        if (tr.sourceLocation != location) continue;
        if (!tr.isEnabled(floorValues, zeroFraction, fractionalOrder))  continue;

        // azzera i clock giusti
        std::set<std::string> zeronew = zeroFraction;
        std::map<std::string, int> floornew = floorValues;
        std::vector<std::vector<std::string>> fonew = fractionalOrder;

        for (const auto clk : tr.resetClocks) {
            zeronew.insert(clk);
            floornew[clk] = 0;
            for (auto& group : fonew) {
                group.erase(std::remove(group.begin(), group.end(), clk), group.end());
            }
            // rimuovi i gruppi vuoti
            fonew.erase(
                std::remove_if(fonew.begin(), fonew.end(),
                               [](const std::vector<std::string>& g){ return g.empty(); }),
                fonew.end()
            );
        }

        // crea la region successiva
        Region next(tr.targetLocation, floornew, zeronew, fonew, maxConstant);
        result.regions.push_back(next);
        result.arches.push_back(RegionTransition(*this, tr.action.action, next));
    }

    return result;
};


RTS Region::discretePredecessors(const std::vector<Transition>& transitions) const {
    RTS result;
    result.regions.push_back(*this);

    for (const auto tr : transitions) {
        // controlla che la transizione finisca nella location corrente
        if (tr.targetLocation != location) continue;

        // controlla che i clock azzerati dalla transizione siano nulli
        // salva la valutazione dei clock azzerati nella mappa reset
        bool flag = false;
        for (const auto clk : tr.resetClocks) {
            if (floorValues.at(clk) != 0 || zeroFraction.find(clk) == zeroFraction.end() ) {
                flag = true;
                break;
            }
        }
        if (flag) continue;

        std::map<std::string, int> floor = floorValues;

        // Base di partenza
        std::set<std::string> baseZero = zeroFraction;
        std::vector<std::vector<std::string>> baseFO = fractionalOrder;

        // Regione base
        auto nuova = Region(tr.sourceLocation, floor, baseZero, baseFO, maxConstant);
        if (tr.isEnabled(floor, baseZero, baseFO)) {
            result.regions.emplace_back(nuova);
            result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
        }

        while (!tr.resetClocks.empty() && std::max_element(
                   floor.begin(), floor.end(),
                   [](const auto& a, const auto& b) {
                       return a.second < b.second;
                   })->second < maxConstant) {

            for (const auto& clk : tr.resetClocks) {

                std::set<std::string> zero = baseZero;
                zero.erase(clk);

                // aggiungi clk come nuovo gruppo in tutte le posizioni
                for (size_t i = 0; i <= baseFO.size(); ++i) {
                    auto fo = baseFO;
                    fo.insert(fo.begin() + i, { clk });
                    nuova = Region(tr.sourceLocation, floor, zero, fo, maxConstant);
                    if (tr.isEnabled(floor, zero, fo)) {
                        result.regions.emplace_back(nuova);
                        result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                    }
                    // Per ogni altro clk1 ≠ clk
                    for (const auto& clk1 : tr.resetClocks) {
                        if (clk1 == clk) continue;
                        std::set<std::string> zero2 = zero;
                        zero2.erase(clk1);

                        // Inserisce clk1 in tutte le posizioni
                        for (size_t j = 0; j <= fo.size(); ++j) {
                            auto fo2 = fo;
                            fo2.insert(fo2.begin() + j, { clk1 });
                            nuova = Region(tr.sourceLocation, floor, zero2, fo2, maxConstant);
                            if (tr.isEnabled(floor, zero2, fo2)) {
                                result.regions.emplace_back(nuova);
                                result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                            }
                            auto floor2 = floor;
                            while(floor2[clk1] < maxConstant - 1) {
                                floor2[clk1] = floor2[clk1] + 1;
                                nuova = Region(tr.sourceLocation, floor2, zero2, fo2, maxConstant);
                                if (tr.isEnabled(floor2, zero2, fo)) {
                                    result.regions.emplace_back(nuova);
                                    result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                                }
                            }

                            // Aggiunge clk1 a gruppi esistenti
                            if (j < fo.size()) {
                                auto fo3 = fo;
                                fo3[j].push_back(clk1);
                                nuova = Region(tr.sourceLocation, floor, zero2, fo3, maxConstant);
                                if (tr.isEnabled(floor, zero2, fo3)) {
                                    result.regions.emplace_back(nuova);
                                    result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                                }
                                auto floor3 = floor;
                                while(floor3[clk1] < maxConstant - 1) {
                                    floor3[clk1] = floor3[clk1] + 1;
                                    nuova = Region(tr.sourceLocation, floor3, zero2, fo3, maxConstant);
                                    if (tr.isEnabled(floor3, zero2, fo3)) {
                                        result.regions.emplace_back(nuova);
                                        result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                                    }
                                }
                            }
                        }
                    }
                }

                // aggiungi clk a gruppi esistenti
                for (size_t i = 0; i < baseFO.size(); ++i) {
                    auto fo = baseFO;
                    fo[i].push_back(clk);
                    std::set<std::string> zero = baseZero;
                    zero.erase(clk);
                    nuova = Region(tr.sourceLocation, floor, zero, fo, maxConstant);
                    if (tr.isEnabled(floor, zero, fo)) {
                        result.regions.emplace_back(nuova);
                        result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                    }

                    for (const auto& clk1 : tr.resetClocks) {
                        if (clk1 == clk) continue;
                        std::set<std::string> zero2 = zero;
                        zero2.erase(clk1);

                        for (size_t j = 0; j <= fo.size(); ++j) {
                            auto fo2 = fo;
                            fo2.insert(fo2.begin() + j, { clk1 });
                            nuova = Region(tr.sourceLocation, floor, zero2, fo2, maxConstant);
                            if (tr.isEnabled(floor, zero2, fo2)) {
                                result.regions.emplace_back(nuova);
                                result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                            }
                            auto floor2 = floor;
                            while(floor2[clk1] < maxConstant - 1) {
                                floor2[clk1] = floor2[clk1] + 1;
                                nuova = Region(tr.sourceLocation, floor2, zero2, fo2, maxConstant);
                                if (tr.isEnabled(floor2, zero2, fo2)) {
                                    result.regions.emplace_back(nuova);
                                    result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                                }
                            }

                            if (j < fo.size()) {
                                auto fo3 = fo;
                                fo3[j].push_back(clk1);
                                nuova = Region(tr.sourceLocation, floor, zero2, fo3, maxConstant);
                                if (tr.isEnabled(floor, zero2, fo3)) {
                                    result.regions.emplace_back(nuova);
                                    result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                                }
                                auto floor3 = floor;
                                while(floor3[clk1] < maxConstant - 1) {
                                    floor3[clk1] = floor3[clk1] + 1;
                                    nuova = Region(tr.sourceLocation, floor3, zero2, fo3, maxConstant);
                                    if (tr.isEnabled(floor3, zero2, fo3)) {
                                        result.regions.emplace_back(nuova);
                                        result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Incrementa i floor dei clock azzerati
            for (const auto& clk : tr.resetClocks) {
                floor[clk]++;
            }
            // Base di partenza
            std::set<std::string> baseZero = zeroFraction;
            std::vector<std::vector<std::string>> baseFO = fractionalOrder;

            // Regione base
            auto nuova = Region(tr.sourceLocation, floor, baseZero, baseFO, maxConstant);
            if (tr.isEnabled(floor, baseZero, baseFO)) {
                result.regions.emplace_back(nuova);
                result.arches.push_back(RegionTransition(nuova, tr.action.action, *this));
            }
        }
    }

    return result;
};


RTS Region::successor(const std::vector<Transition>& transitions) const {
    RTS result = discreteSuccessors(transitions);

    auto delay = delaySuccessor();   // delay è un std::optional<Region>
    if (delay.has_value()) {         // controlla se esiste un successore di delay
        result.regions.insert(result.regions.begin(), delay.value());
        result.arches.insert(result.arches.begin(), RegionTransition(*this, "tau", delay.value()));
    }

    return result;
}


RTS Region::predecessor(const std::vector<Transition>& transitions) const {
    RTS result = discretePredecessors(transitions);
    auto delay = delayPredecessor();   // delay è un std::optional<Region>
    if (delay.has_value()) {         // controlla se esiste un successore di delay
        result.regions.insert(result.regions.begin(), delay.value());
        result.arches.insert(result.arches.begin(), RegionTransition(*this, "tau", delay.value()));
    }
    return result;
}


void Region::print() const {
    std::cout << "Location: " << location << "\n";
    std::cout << "Floor values:\n";
    for (const auto& [clk, val] : floorValues) {
        std::cout << "  " << clk << ": " << val << "\n";
    }

    std::cout << "Zero fraction clocks: ";
    for (const auto& clk : zeroFraction) {
        std::cout << clk << " ";
    }
    std::cout << "\nFractional order:\n";
    for (const auto& group : fractionalOrder) {
        std::cout << "  [";
        for (size_t i = 0; i < group.size(); ++i) {
            std::cout << group[i];
            if (i != group.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
}

std::string Region::ID() const {
    std::ostringstream oss;
    oss << location << "|";

    // floorValues
    for (const auto& [clk, val] : floorValues) {
        oss << clk << ":" << val << ",";
    }
    oss << "|";

    // zeroFraction
    for (const auto& clk : zeroFraction) {
        oss << clk << ",";
    }
    oss << "|";

    // fractionalOrder
    for (const auto& group : fractionalOrder) {
        oss << "[";
        for (const auto& clk : group) {
            oss << clk << ",";
        }
        oss << "]";
    }

    // maxConstant
    oss << "|maxConst:" << maxConstant;

    return oss.str();
}

std::string Region::clockID() const {
    std::ostringstream oss;

    // floorValues
    for (const auto& [clk, val] : floorValues) {
        oss << clk << ":" << val << ",";
    }
    oss << "|";

    // zeroFraction
    for (const auto& clk : zeroFraction) {
        oss << clk << ",";
    }
    oss << "|";

    // fractionalOrder
    for (const auto& group : fractionalOrder) {
        oss << "[";
        for (const auto& clk : group) {
            oss << clk << ",";
        }
        oss << "]";
    }

    return oss.str();
}

