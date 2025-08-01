//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "TimedArena.h"

TimedArena::State::State(const Region& region, Player player):
  region(region), player(player){}

TimedArena::TransitionRelation::TransitionRelation(const State& state, double second, const State& ending):
  initial(state), transition(second), ending(ending){}




