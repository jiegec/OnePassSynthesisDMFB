#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <z3++.h>
#include <vector>
#include "Graph.h"

class Solver {
 public:
  Solver(z3::context& c, const Graph& graph, int width, int height, int time);
  z3::solver get();
  void print(const z3::model & model);

 private:
  void add_consistency(z3::context &c);
  void add_placement(z3::context &c);
  void add_movement(z3::context &c);


  z3::solver solver;
  std::vector<std::vector<std::vector<std::vector<z3::expr>>>>
      c;  // c_{x,y,i}^t
  int width;
  int height;
  int time;
  const Graph &graph;
  std::vector<z3::expr> sink;
  std::vector<std::vector<z3::expr>> dispenser;
  std::vector<std::vector<std::vector<z3::expr>>> detector;
};

#endif
