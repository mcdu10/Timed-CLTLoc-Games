//
// Created by Maria Carmen D'urbano on 30/08/25.
//

#ifndef TIMEDCLTLOCGAME_H
#define TIMEDCLTLOCGAME_H

#include"TimedArena.h"
#include"CLTLocFormula.h"

class TCG {
public:
    TAr A;
    CLTLocFormula winningCondition;

    TCG() = default;

    TCG(const TAr& arena, const CLTLocFormula& winCond)
        : A(arena), winningCondition(winCond) {}



    std::vector<RegionTransition> Omega(std::vector<Region> regions);
    std::vector<RegionTransition> Deltac(std::vector<Region> regions);
    std::vector<RegionTransition> Deltae(std::vector<Region> regions);
    std::vector<RegionTransition> OmegaOmega(std::vector<Region> regions);
    std::vector<Region> Pi(std::vector<Region> regions);

    RTS algorithm1();

};

#endif //TIMEDCLTLOCGAME_H
