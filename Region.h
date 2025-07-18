//
// Created by Maria Carmen D'urbano on 12/07/25.
//

#ifndef REGION_H
#define REGION_H

#include <map>
#include <set>
#include <vector>
#include <string>
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

    // Verifica se due regioni sono equivalenti
    bool isEquivalentTo(const Region& other) const;

    // Restituisce un rappresentante della regione
    std::map<std::string, double> getRepresentativeValuation() const;

    // Restituisce le regioni raggiungibili facendo passare il tempo
    std::vector<Region> delaySuccessors() const;

    // Restituisce le regioni raggiungibili tramite le transizioni (in input)
    std::vector<Region> discreteSuccessors(const std::vector<Transition>& transitions) const;

    // Restituisce le regioni dalle quali si può raggiungere la corrente facendo passare il tempo
    std::vector<Region> delayPredecessors() const;

    // Restituisce le regioni dalle quali si può raggiungere la corrente tramite le transizioni (in input)
    std::vector<Region> discretePredecessors(const std::vector<Transition>& transitions) const;

    // Restituisce tutte le regioni dalle quali si può raggiungere la corrente (facendo passare il tempo o con transizioni)
    std::vector<Region> predecessor(const std::vector<Transition>& transitions) const;


    // Stampa la regione
    void print() const;
};

#endif //REGION_H
