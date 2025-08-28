//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "TimedArena.h"

#include <deque>
#include <functional>


TimedArena::TAr::TAr(const Region& Reg, const std::vector<Transition>& tr):
  R(Reg), transitions(tr){}

TimedArena::TAr::TAr(const std::vector<std::string>& locations,
     const std::set<std::string>& clocks,
     const std::vector<Transition>& trans,
     const int max)
    : transitions(trans)
{
  // floorValues inizializzati a 0
  std::map<std::string,int> floorValues;
  for (const auto& clk : clocks) {
    floorValues[clk] = 0;
  }

  // Regione iniziale nella prima location
  std::string startLoc = locations.empty() ? "loc0" : locations[0];
  R = Region(startLoc, floorValues, clocks, {}, max);
}

enum class Color {black, grey};

RTS TimedArena::TAr::BFS(std::function<RTS(const Region&)> neighborFunc) {
  RTS result;

  // Creo la coda ed inserisco la regione iniziale
  std::deque<Region> queue;
  queue.push_back(R);

  std::map<std::string, Color> exp;
  exp[R.ID()] = Color::grey;
  while (!queue.empty()) {
    Region current = queue.front();
    queue.pop_front();
    RTS neighbor = neighborFunc(current);
    for (const auto& t : neighbor.regions) {
      if (exp.find(t.ID()) == exp.end()) {
        exp[t.ID()] = Color::grey;
        queue.push_back(t);
      }
    }
    for (const auto& arc : neighbor.arches) {
      result.arches.push_back(arc);
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
  std::deque<Region> queue;
  queue.push_back(R);

  std::map<std::string, Col> exp;
  exp[R.ID()] = Col::grey;
  exp[r.ID()] = Col::white;
  while (!queue.empty()) {
    Region current = queue.front();
    queue.pop_front();
    std::vector<Region> adjacent = current.predecessor(transitions).regions;
    std::vector<Region> succ = current.successor(transitions).regions;
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



void TimedArena::TAr::print() {
  std::cout << "Initial region: " << std::endl;
  R.print();
  std::cout << "Transitions: " << std::endl;
  for (const auto& t : transitions) {
    t.print();
  }
};



