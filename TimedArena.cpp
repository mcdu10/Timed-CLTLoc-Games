//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "TimedArena.h"

#include <functional>


TimedArena::RegionTransition::RegionTransition(const Region& state1, double time, const Region& state2):
  source(state1), transition(time), target(state2){}

TimedArena::RegionTransition::RegionTransition(const Region& state1, Act action, const Region& state2):
  source(state1), transition(action), target(state2){}

void TimedArena::RTS::printRTS() {
    std::cout << "=== Regions ===\n";
    for (size_t i = 0; i < regions.size(); ++i) {
      std::cout << "\n";
      std::cout << "R" << i << ": " << "\n";
      regions[i].print();
    }

    std::cout << "\n=== Transitions ===\n";
    for (const auto& t : arches) {
      // trova indice sorgente e target
      auto itSource = std::find_if(regions.begin(), regions.end(),
          [&](const Region& r) { return r.isEquivalentTo(t.source); });
      auto itTarget = std::find_if(regions.begin(), regions.end(),
          [&](const Region& r) { return r.isEquivalentTo(t.target); });

      int idxSource = (itSource != regions.end()) ? (itSource - regions.begin()) : -1;
      int idxTarget = (itTarget != regions.end()) ? (itTarget - regions.begin()) : -1;

      std::cout << "R" << idxSource << " --(";
      if (std::holds_alternative<Act>(t.transition)) {
        std::cout << std::get<Act>(t.transition).action;
      } else {
        std::cout << "delay=" << std::get<double>(t.transition);
      }
      std::cout << ")--> R" << idxTarget << "\n";
    }
}



TimedArena::TAr::TAr(const Region& Reg, const std::vector<Transition>& tr):
  R(Reg), transitions(tr){}

enum class Color {black, grey};

TimedArena::RTS TimedArena::TAr::BFS(std::function<std::vector<Region>(const Region&)> neighborFunc) {
  RTS result;

  // Creo la coda ed inserisco la regione iniziale
  std::vector<Region> queue;
  queue.push_back(R);

  std::map<std::string, Color> exp;
  exp[R.ID()] = Color::grey;
  while (!queue.empty()) {
    Region current = queue.front();
    queue.erase(queue.begin());
    for (const auto& t : neighborFunc(current)) {
      if (exp.find(t.ID()) == exp.end()) {
        exp[t.ID()] = Color::grey;
        queue.push_back(t);
      }
    }
    exp[current.ID()] = Color::black;
    result.regions.push_back(current);
  }
  return result;
}

enum class Col {black, white, grey};

bool TimedArena::TAr::reachable(const Region& r) {
  RTS result;

  // Creo la coda ed inserisco la regione iniziale
  std::vector<Region> queue;
  queue.push_back(R);

  std::map<std::string, Col> exp;
  exp[R.ID()] = Col::grey;
  exp[r.ID()] = Col::white;
  while (!queue.empty()) {
    Region current = queue.front();
    queue.erase(queue.begin());
    std::vector<Region> adjacent = current.predecessor(transitions);
    std::vector<Region> succ = current.successor(transitions);
    adjacent.insert(adjacent.end(), succ.begin(), succ.end());
    for (const auto& t : adjacent) {
      if (exp.find(t.ID()) == exp.end()) {
        exp[t.ID()] = Col::grey;
        queue.push_back(t);
      }
      if (exp[t.ID()] == Col::white) {
        return 1;
      }
    }
    exp[current.ID()] = Col::black;
    result.regions.push_back(current);
  }
  return 0;
};






