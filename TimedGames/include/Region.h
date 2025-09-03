//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#ifndef REGION_H
#define REGION_H

#include <map>
#include <optional>
#include <set>
#include <vector>
#include <string>
#include <variant>
#include "Transition.h"

struct RTS;



class Region {
public:
    std::string location;
    std::map<std::string, int> floorValues;  // Integer part
    std::set<std::string> zeroFraction;  // Clock with zero fractional part
    std::vector<std::vector<std::string>> fractionalOrder;  // Ordering of fractional parts
    int maxConstant; // Max const with which the clocks are compared

    // Constructors:
    Region(const std::map<std::string, double>& valuation, const std::string& loc, int maxConst);
    Region(const std::string& loc, const std::map<std::string, int>& floor, const std::set<std::string>& zero, const std::vector<std::vector<std::string>>& fo, int max_constant);
    Region();

    // Verifica se due regioni sono equivalenti
    bool isEquivalentTo(const Region& other) const;

    // Verifica se in un vettore di regioni ce n'è una equivalente
    static bool containsRegionEquivalentTo(const std::vector<Region>& vec, const Region& target);

    // Restituisce un rappresentante della regione
    std::map<std::string, double> getRepresentativeValuation() const;

    // Restituisce le regioni raggiungibili facendo passare il tempo
    std::optional<Region> delaySuccessor() const;

    // Restituisce le regioni raggiungibili tramite le transizioni (in input)
    RTS discreteSuccessors(const std::vector<Transition>& transitions) const;

    // Restituisce tutte le regioni raggiungibili
    RTS successor(const std::vector<Transition>& transitions) const;

    // Restituisce le regioni dalle quali si può raggiungere la corrente facendo passare il tempo
    std::optional<Region> delayPredecessor() const;

    // Restituisce le regioni dalle quali si può raggiungere la corrente tramite le transizioni (in input)
    RTS discretePredecessors(const std::vector<Transition>& transitions) const;

    // Restituisce tutte le regioni dalle quali si può raggiungere la corrente (facendo passare il tempo o con transizioni)
    RTS predecessor(const std::vector<Transition>& transitions) const;


    // Stampa la regione
    void print() const;

    // ID per identificare univocamente una regione
    std::string ID() const;

    std::string clockID() const;
    RTS discretePredecessorsLight(const std::vector<Transition>& transitions) const;
    RTS predecessorLight(const std::vector<Transition>& transitions) const;

    bool operator==(const Region& other) const {
        return this->ID() == other.ID();
    }



};


#endif //REGION_H
