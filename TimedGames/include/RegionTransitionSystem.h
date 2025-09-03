//
// Created by Maria Carmen D'urbano on 27/08/25.
//

#ifndef REGIONTRANSITIONSYSTEM_H
#define REGIONTRANSITIONSYSTEM_H
#include "Region.h"

struct RegionTransition {
    Region source;
    std::string transition;
    Region target;

    RegionTransition();
    RegionTransition(const Region& state1, std::string action, const Region& state2);

    bool operator<(const RegionTransition& other) const {
        if (source.ID() != other.source.ID() ) return source.ID() < other.source.ID();
        if (transition != other.transition) return transition < other.transition;
        return target.ID() < other.target.ID();
    }

    bool operator==(const RegionTransition& other) const {
        return source == other.source &&
               transition == other.transition &&
               target == other.target;
    }

};

struct RTS {
    std::vector<Region> regions;
    std::vector<RegionTransition> arches;

    void printRTS();
};

#endif //REGIONTRANSITIONSYSTEM_H
