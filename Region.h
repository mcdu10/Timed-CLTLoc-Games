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


class Region {
public:
    std::string location;
    std::map<std::string, int> floorValues;  // Parte intera dei clock
    std::set<std::string> zeroFraction;  // Clock con parte frazionaria zero
    std::vector<std::vector<std::string>> fractionalOrder;  // Ordine tra le frazioni
    int maxConstant;

    // Costruttore: crea una regione a partire da una valutazione dei clock
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
    std::vector<Region> discreteSuccessors(const std::vector<Transition>& transitions) const;

    // Restituisce tutte le regioni raggiungibili
    std::vector<Region> successor(const std::vector<Transition>& transitions) const;

    // Restituisce le regioni dalle quali si può raggiungere la corrente facendo passare il tempo
    std::optional<Region> delayPredecessor() const;

    // Restituisce le regioni dalle quali si può raggiungere la corrente tramite le transizioni (in input)
    std::vector<Region> discretePredecessors(const std::vector<Transition>& transitions) const;

    // Restituisce tutte le regioni dalle quali si può raggiungere la corrente (facendo passare il tempo o con transizioni)
    std::vector<Region> predecessor(const std::vector<Transition>& transitions) const;


    // Stampa la regione
    void print() const;

    // ID per identificare univocamente una regione
    std::string ID() const;


    bool operator<(const Region& other) const {
        if (location != other.location) return location < other.location;
        if (floorValues != other.floorValues) return floorValues < other.floorValues;
        if (zeroFraction != other.zeroFraction) return zeroFraction < other.zeroFraction;
        if (fractionalOrder != other.fractionalOrder) return fractionalOrder < other.fractionalOrder;
        return maxConstant < other.maxConstant;
    };
    bool operator==(const Region& other) const {
        return isEquivalentTo(other);
    }
};

#endif //REGION_H
