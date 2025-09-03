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

    // Check whether two regions are equivalent
    bool isEquivalentTo(const Region& other) const;

    // Check whether an equivalent region exists in a vector of regions
    static bool containsRegionEquivalentTo(const std::vector<Region>& vec, const Region& target);

    // Return a representative of the region
    std::map<std::string, double> getRepresentativeValuation() const;

    // Return the regions reachable by letting time elapse
    std::optional<Region> delaySuccessor() const;

    // Return the regions reachable via the (input) transitions
    RTS discreteSuccessors(const std::vector<Transition>& transitions) const;

    // Return all reachable regions
    RTS successor(const std::vector<Transition>& transitions) const;

    // Return the regions from which the current one can be reached by letting time elapse
    std::optional<Region> delayPredecessor() const;

    // Return the regions from which the current one can be reached through (input) transitions
    RTS discretePredecessors(const std::vector<Transition>& transitions) const;

    // Return all regions from which the current one can be reached (either by letting time elapse or via transitions)
    RTS predecessor(const std::vector<Transition>& transitions) const;


    // Print the region
    void print() const;

    // ID to uniquely identify a region
    std::string ID() const;

    std::string clockID() const;

    bool operator==(const Region& other) const {
        return this->ID() == other.ID();
    }



};


#endif //REGION_H
