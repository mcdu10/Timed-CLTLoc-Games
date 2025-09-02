//
// Created by Maria Carmen D'urbano on 15/07/25.
//

#include "TimedArena.h"

#include <deque>
#include <functional>


TAr::TAr(const std::map<std::string, PLAYER> loc, const Region& Reg, const std::vector<Transition>& tr):
  locations(loc), R(Reg), transitions(tr){}

TAr::TAr(const std::map<std::string, PLAYER>& loc,
     const std::set<std::string>& clocks,
     const std::vector<Transition>& trans,
     const int max)
    : transitions(trans)
{
  locations = loc;
  // floorValues initialized to 0
  std::map<std::string,int> floorValues;
  for (const auto& clk : clocks) {
    floorValues[clk] = 0;
  }

  // Starting region in the first location
  std::string startLoc = locations.empty() ? "loc0" : locations.begin()->first;
  R = Region(startLoc, floorValues, clocks, {}, max);
}

enum class Color {black, grey};

RTS TAr::BFS(std::function<RTS(const Region&)> neighborFunc) {
  RTS result;

  // Creation of the queue and insertion of the starting region
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
    for (auto& arc : neighbor.arches) {
      result.arches.push_back(arc);
    }
    exp[current.ID()] = Color::black;
    result.regions.push_back(current);
  }
  return result;
}

enum class Col {black, white, grey};

bool TAr::reachable(const Region& r) {
  RTS result;

  // Creation of the queue and insertion of the starting region
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



void TAr::print() {
  std::cout << "Initial region: " << std::endl;
  R.print();
  std::cout << "\nTransitions: " << std::endl;
  for (const auto& t : transitions) {
    t.print();
  }
};



