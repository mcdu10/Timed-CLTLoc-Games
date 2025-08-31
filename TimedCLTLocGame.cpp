//
// Created by Maria Carmen D'urbano on 30/08/25.
//

#include "TimedCLTLocGame.h"


std::vector<RegionTransition> TCG::Omega(std::vector<Region> regions) {
    std::vector<RegionTransition> result;
    std::vector<Transition> tr = A.transitions;
    auto neighborsFunc = [&tr](const Region& r) -> RTS {
        RTS succ = r.successor(tr);
        RTS pred = r.predecessor(tr);
        RTS result = succ;
        for (const auto& reg : pred.regions) {
            result.regions.push_back(reg);
        }
        return result;
    };
    RTS Ra = A.BFS(neighborsFunc);
    std::vector<RegionTransition> arch = Ra.arches;
    std::map<RegionTransition, bool> found;
    for (auto tr : arch) {
        found[tr] = false;
        if (find(regions.begin(), regions.end(), tr.target) != regions.end()) {
            found[tr] = true;
        }
    }
    for (auto act : found) {
        if (!act.second) {
            result.push_back(act.first);
        }
    }
    return result;
}

std::vector<RegionTransition> TCG::Deltac(std::vector<Region> regions) {
    std::vector<RegionTransition> result;
    std::vector<Transition> tr = A.transitions;
    auto neighborsFunc = [&tr](const Region& r) -> RTS {
        RTS succ = r.successor(tr);
        RTS pred = r.predecessor(tr);
        RTS result = succ;
        for (const auto& reg : pred.regions) {
            result.regions.push_back(reg);
        }
        return result;
    };
    RTS Ra = A.BFS(neighborsFunc);
    std::vector<RegionTransition> arch = Ra.arches;
    for (auto tr : arch) {
        result.push_back(tr);
    }
    return result;
}

std::vector<RegionTransition> TCG::Deltae(std::vector<Region> regions) {
    std::vector<RegionTransition> result;
    std::vector<Transition> tr = A.transitions;
    auto neighborsFunc = [&tr](const Region& r) -> RTS {
        RTS succ = r.successor(tr);
        RTS pred = r.predecessor(tr);
        RTS result = succ;
        for (const auto& reg : pred.regions) {
            result.regions.push_back(reg);
        }
        return result;
    };
    RTS Ra = A.BFS(neighborsFunc);
    std::vector<RegionTransition> arch = Ra.arches;
    std::map<RegionTransition, bool> found;
    for (auto tr : arch) {
        found[tr] = false;
        if (find(regions.begin(), regions.end(), tr.target) != regions.end()) {
            found[tr] = true;
        }
    }
    for (auto act : found) {
        if (!act.second && (A.locations[act.first.source.location] == PLAYER::environment) && (find(regions.begin(), regions.end(), act.first.target) != regions.end())) {
            result.push_back(act.first);
        }
    }

    return result;
}

std::vector<RegionTransition> TCG::OmegaOmega(std::vector<Region> regions) {
    std::vector<RegionTransition> result = Omega(regions);
    std::vector<RegionTransition> deltaC = Deltac(regions);
    std::vector<RegionTransition> deltaE = Deltae(regions);

    result.insert(result.end(), deltaC.begin(), deltaC.end());
    result.insert(result.end(), deltaE.begin(), deltaE.end());

    return result;
}


std::vector<Region> TCG::Pi(std::vector<Region> regions) {
    std::vector<Region> result;
    for (auto tr : OmegaOmega(regions)) {
        if (find(result.begin(), result.end(), tr.source) == result.end()) {
            result.push_back(tr.source);
        }
    }
    return result;
}

RTS TCG::algorithm1() {
    RTS result;
    std::vector<Transition> tr = A.transitions;
    auto neighborsFunc = [&tr](const Region& r) -> RTS {
        RTS succ = r.successor(tr);
        RTS pred = r.predecessor(tr);
        RTS resul = succ;
        for (const auto& reg : pred.regions) {
            resul.regions.push_back(reg);
        }
        return resul;
    };
    RTS Ra = A.BFS(neighborsFunc);
    std::cout << "ancora ok dopo BFS A" << std::endl;
    std::vector<Region> S;
    std::vector<Region> G;
    winningCondition.phi.print();
    winningCondition.psi.print();
    for (auto r : Ra.regions) {
        if (winningCondition.phiEnabled(r)) {
            S.push_back(r);
        }
        if (winningCondition.psiEnabled(r)) {
            G.push_back(r);
        }
    }
    std::cout << "ancora ok dopo S, G" << std::endl;
    std::cout << S.size() << std::endl;
    std::cout << G.size() << std::endl;
    for (auto r : S) {
        r.print();
    }
    for (auto r : G) {
        r.print();
    }
    std::vector<Region> barG;
    std::cout << "ok prima di bar G" << std::endl;
    while(barG != G) {
        std::cout << "barG size = " << barG.size()
           << ", G size = " << G.size() << std::endl;

        barG.clear();
        barG.insert(barG.end(), G.begin(), G.end());
        std::cout << "barG size = " << barG.size()
           << ", G size = " << G.size() << std::endl;
        std::vector<Region> barS = Pi(barG);

        std::cout << "barS size = " << barS.size() << std::endl;
        for (auto r : barS) {
            if (find(S.begin(), S.end(), r) != S.end() && std::find(G.begin(), G.end(), r) == G.end()) {
                G.push_back(r);
            }
        }
        std::cout << "barG size = " << barG.size()
           << ", G size = " << G.size() << std::endl;
    }
    std::cout << "ok dopo while bar G" << std::endl;
    for (auto r : G) {
        r.print();
    }
    A.R.print();
    if (find(G.begin(), G.end(), A.R) != G.end()) {
        std::cout << "esiste" << std::endl;
        result.regions = G;
        std::vector<RegionTransition> arch = OmegaOmega(G);
        for (auto a : Ra.arches) {
            if (find(G.begin(), G.end(), a.source) != G.end() && find(G.begin(), G.end(), a.target) != G.end() &&
                find(arch.begin(), arch.end(), a) != arch.end()) {
                result.arches.push_back(a);
                }
        }
    }

    return result;
}



