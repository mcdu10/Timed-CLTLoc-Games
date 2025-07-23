//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#include "Region.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <variant>
#include <_strings.h>

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


Region Region::delaySuccessor() const {
    std::vector<std::vector<std::string>> fo;
    std::set<std::string> zero;

    if (!zeroFraction.empty()) {
        fo = fractionalOrder;
        fo.insert(fo.begin(), std::vector<std::string>(zeroFraction.begin(), zeroFraction.end()));
        return Region(location, floorValues, zero, fo, maxConstant);
    }

    std::vector<std::string> greater = fractionalOrder.back();
    zero.insert(greater.begin(), greater.end());
    fo = std::vector<std::vector<std::string>>(fractionalOrder.begin(), fractionalOrder.end()-1);

    std::map<std::string, int> floor = floorValues;
    for (auto it : zero) {
        floor[it] = floor[it] + 1;
    }
    return Region(location, floor, zero, fo, maxConstant);
}


Region Region::delayPredecessor() const {

    std::string minclk = std::min_element(
    floorValues.begin(), floorValues.end(),
    [](const auto& a, const auto& b) {
        return a.second < b.second;
    })->first;

    if (floorValues.at(minclk) == 0 && zeroFraction.find(minclk) != zeroFraction.end()) {
        return Region(location, floorValues, zeroFraction, fractionalOrder, maxConstant);
    }

    std::set<std::string> zero;
    std::vector<std::vector<std::string>> fo = fractionalOrder;
    std::map<std::string, int> floor = floorValues;
    if (!zeroFraction.empty()) {
        std::vector<std::string> lower = std::vector<std::string>(zeroFraction.begin(), zeroFraction.end());
        fo.insert(fo.end(), lower.begin(), lower.end());
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


std::vector<Region> Region::discreteSuccessors(const std::vector<Transition>& transitions) const {
    std::vector<Region> result;
    std::map<std::string, double> sampleVal = getRepresentativeValuation();

    for (const auto tr : transitions) {
        // controlla che la transizione parta dalla location corrente e che le
        // relative guardie siano soddisfatte
        if (tr.sourceLocation != location) continue;
        if (!tr.isEnabled(sampleVal))  continue;

        // azzera i clock giusti
        std::map<std::string, double> update = sampleVal;
        for (const auto clk : tr.resetClocks) {
            update[clk] = 0;
        }

        // crea la region successiva
        Region next(update, tr.targetLocation, maxConstant);
        result.push_back(next);
    }

    return result;
};


std::vector<Region> Region::discretePredecessors(const std::vector<Transition>& transitions) const {
    std::vector<Region> result;

    for (const auto tr : transitions) {
        // controlla che la transizione finisca nella location corrente
        if (tr.targetLocation != location) continue;

        // controlla che i clock azzerati dalla transizione siano nulli
        // salva la valutazione dei clock azzerati nella mappa reset
        bool flag = false;
        for (const auto clk : tr.resetClocks) {
            if (floorValues.at(clk) != 0 || zeroFraction.find(clk) != zeroFraction.end() ) {
                flag = true;
                break;
            }
        }
        if (flag) continue;

        std::map<std::string, int> floor = floorValues;

        while (std::max_element(
                   floor.begin(), floor.end(),
                   [](const auto& a, const auto& b) {
                       return a.second < b.second;
                   })->second < maxConstant) {

            // Base di partenza
            std::set<std::string> baseZero = zeroFraction;
            std::vector<std::vector<std::string>> baseFO = fractionalOrder;

            // Regione base
            auto nuova = Region(location, floor, baseZero, baseFO, maxConstant);
            if (!containsRegionEquivalentTo(result, nuova)) {
                result.emplace_back(nuova);
            }

            for (const auto& clk : tr.resetClocks) {
                std::set<std::string> zero = baseZero;
                zero.erase(clk);

                // aggiungi clk come nuovo gruppo in tutte le posizioni
                for (size_t i = 0; i <= baseFO.size(); ++i) {
                    auto fo = baseFO;
                    fo.insert(fo.begin() + i, { clk });
                    nuova = Region(location, floor, zero, fo, maxConstant);
                    if (!containsRegionEquivalentTo(result, nuova)) {
                        result.emplace_back(nuova);
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
                            nuova = Region(location, floor, zero2, fo2, maxConstant);
                            if (!containsRegionEquivalentTo(result, nuova)) {
                                result.emplace_back(nuova);
                            }
                            auto floor2 = floor;
                            while(floor2[clk1] < maxConstant) {
                                floor2[clk1] = floor2[clk1] + 1;
                                nuova = Region(location, floor2, zero2, fo2, maxConstant);
                                if (!containsRegionEquivalentTo(result, nuova)) {
                                    result.emplace_back(nuova);
                                }
                            }

                            // Aggiunge clk1 a gruppi esistenti
                            if (j < fo.size()) {
                                auto fo3 = fo;
                                fo3[j].push_back(clk1);
                                nuova = Region(location, floor, zero2, fo3, maxConstant);
                                if (!containsRegionEquivalentTo(result, nuova)) {
                                    result.emplace_back(nuova);
                                }
                                auto floor3 = floor;
                                while(floor3[clk1] < maxConstant) {
                                    floor3[clk1] = floor3[clk1] + 1;
                                    nuova = Region(location, floor3, zero2, fo3, maxConstant);
                                    if (!containsRegionEquivalentTo(result, nuova)) {
                                        result.emplace_back(nuova);
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
                    nuova = Region(location, floor, zero, fo, maxConstant);
                    if (!containsRegionEquivalentTo(result, nuova)) {
                        result.emplace_back(nuova);
                    }

                    for (const auto& clk1 : tr.resetClocks) {
                        if (clk1 == clk) continue;
                        std::set<std::string> zero2 = zero;
                        zero2.erase(clk1);

                        for (size_t j = 0; j <= fo.size(); ++j) {
                            auto fo2 = fo;
                            fo2.insert(fo2.begin() + j, { clk1 });
                            nuova = Region(location, floor, zero2, fo2, maxConstant);
                            if (!containsRegionEquivalentTo(result, nuova)) {
                                result.emplace_back(nuova);
                            }
                            auto floor2 = floor;
                            while(floor2[clk1] < maxConstant) {
                                floor2[clk1] = floor2[clk1] + 1;
                                nuova = Region(location, floor2, zero2, fo2, maxConstant);
                                if (!containsRegionEquivalentTo(result, nuova)) {
                                    result.emplace_back(nuova);
                                }
                            }

                            if (j < fo.size()) {
                                auto fo3 = fo;
                                fo3[j].push_back(clk1);
                                nuova = Region(location, floor, zero2, fo3, maxConstant);
                                if (!containsRegionEquivalentTo(result, nuova)) {
                                    result.emplace_back(nuova);
                                }
                                auto floor3 = floor;
                                while(floor3[clk1] < maxConstant) {
                                    floor3[clk1] = floor3[clk1] + 1;
                                    nuova = Region(location, floor3, zero2, fo3, maxConstant);
                                    if (!containsRegionEquivalentTo(result, nuova)) {
                                        result.emplace_back(nuova);
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
        }
    }

    return result;
};


std::vector<Region> Region::successor(const std::vector<Transition>& transitions) const {
    std::vector<Region> result = discreteSuccessors(transitions);
    result.push_back(delaySuccessor());
    return result;
}


std::vector<Region> Region::predecessor(const std::vector<Transition>& transitions) const {
    std::vector<Region> result = discretePredecessors(transitions);
    result.push_back(delayPredecessor());
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
