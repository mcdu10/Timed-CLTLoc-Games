//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#include "Region.h"
#include <iostream>
#include <cmath>
#include <algorithm>

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


std::vector<Region> Region::delaySuccessors() const {
    std::vector<Region> result;
    std::map<std::string, double> sampleVal = getRepresentativeValuation();

    // incrementa la valutazione dei clock di 0.01
    double epsilon = 0.01;
    for (int i = 1; i <= 100; ++i) {
        double t = i * epsilon;
        std::map<std::string, double> advanced;

        for (const auto& [clk, val] : sampleVal) {
            advanced[clk] = val + t;
        }

        Region next(advanced, location, maxConstant);

        // evita duplicati
        bool flag = false;
        for (auto it = result.begin(); it != result.end(); ++it) {
            if (next.isEquivalentTo(*it)) {
                flag = true;
                break;
            }
        }
        if (!flag && !isEquivalentTo(next)) result.push_back(next);
    }

    return result;
}


std::vector<Region> Region::delayPredecessors() const {
    std::vector<Region> result;
    std::map<std::string, double> sampleVal = getRepresentativeValuation();

    double minclk = std::min_element(
    sampleVal.begin(), sampleVal.end(),
    [](const auto& a, const auto& b) {
        return a.second < b.second;
    })->second;

    // decrementa la valuazione del clock di 0.01 (controllando che non diventi negativa)
    double epsilon = 0.01;
    for (int i = 1; i <= 100 && minclk >= i*epsilon; ++i) {
        double t = i * epsilon;
        std::map<std::string, double> earlier;

        for (const auto& [clk, val] : sampleVal) {
            earlier[clk] = val - t;
        }

        Region next(earlier, location, maxConstant);

        // evita duplicati
        bool flag = false;
        for (auto it = result.begin(); it != result.end(); ++it) {
            if (next.isEquivalentTo(*it)) {
                flag = true;
                break;
            }
        }
        if (!flag && !isEquivalentTo(next)) result.push_back(next);
    }

    return result;
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
    std::map<std::string, double> sampleVal = getRepresentativeValuation();

    for (const auto tr : transitions) {
        // controlla che la transizione finisca nella location corrente
        if (tr.targetLocation != location) continue;

        // controlla che i clock azzerati dalla transizione siano nulli
        // salva la valutazione dei clock azzerati nella mappa reset
        std::map<std::string, double> reset;
        bool flag = false;
        for (const auto clk : tr.resetClocks) {
            if (sampleVal[clk] != 0) {
                flag = true;
                break;
            }
            reset[clk] = 0;
        }
        if (flag) continue;

        // crea una regione considerando solo i clock che sono stati azzerati
        // in modo da poterli incrementare (tramite delaySuccessors)
        // per trovare i predecessori
        Region res(reset, location, maxConstant);
        std::vector<Region> test = res.delaySuccessors();
        std::map<std::string, double> val = sampleVal;
        // tra tutte le valutazioni (incrementate con delaySuccessor) dei clock
        // che sono stati azzerati controlla quali soddisferebbero le guardie
        for (auto it = test.begin(); it != test.end(); ++it) {
            reset = it->getRepresentativeValuation();
            for (const auto clk : tr.resetClocks) {
                val[clk] = reset[clk];
            }
            if (tr.isEnabled(val)) {
                Region candidate(val, tr.sourceLocation, maxConstant);

                // evita duplicati
                bool exists = false;
                for (const auto& r2 : result) {
                    if (r2.isEquivalentTo(candidate)) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    result.push_back(candidate);
                }
            }
        }
    }

    return result;
};

std::vector<Region> Region::predecessor(const std::vector<Transition>& transitions) const {
    std::vector<Region> result = discretePredecessors(transitions);
    result.insert(result.end(), delayPredecessors().begin(), delayPredecessors().end());

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
