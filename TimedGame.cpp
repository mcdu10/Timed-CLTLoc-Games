//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "TimedGame.h"

std::vector<TransitionRelation> TimedGame::Omega (TAr A, std::vector<State> G) {
    std::vector<TransitionRelation> result;
    for (int i = 0; i < G.size(); i++) {
        State ending(G[i].region, G[i].player);
        std::map<Region, double> predecessors;
        for (auto it : predecessors) {
            State starting(it.first, Player::Controller);
            result.push_back(TransitionRelation(starting, it.second, ending));
        }
    }
    return result;

}