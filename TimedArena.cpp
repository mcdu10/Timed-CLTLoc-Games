//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "TimedArena.h"

TimedArena::State::State(const Region& region):
  region(region){}

void TimedArena::State::print() const {
  std::cout << "location: " << region.location;
  region.print();
}



TimedArena::RegionTransition::RegionTransition(const State& state1, double time, const State& state2):
  source(state1), transition(time), target(state2){}

TimedArena::RegionTransition::RegionTransition(const State& state1, Act action, const State& state2):
  source(state1), transition(action), target(state2){}



TimedArena::TAr::TAr(const std::vector<Act>& act, const std::vector<std::string>& loc,
  const std::string& loc0, const std::vector<Clock>& clk, const std::vector<Transition>& tr):
  actions(act), locations(loc), initialLocation(loc0), clocks(clk), transitions(tr){}







