// Copyright (C) 2018 Jiajie Chen
// 
// This file is part of OnePassSynthesisDMFB.
// 
// OnePassSynthesisDMFB is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// OnePassSynthesisDMFB is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with OnePassSynthesisDMFB.  If not, see <http://www.gnu.org/licenses/>.
// 

#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <z3++.h>
#include <vector>
#include "Graph.h"

class Solver {
 public:
  Solver(z3::context& c, const Graph& graph, int width, int height, int time, int max_points);
  z3::solver get_solver();
  int get_num_points();
  void print(const z3::model & model);

 private:
  void add_consistency(z3::context &c);
  void add_placement(z3::context &c);
  void add_movement(z3::context &c);


  z3::solver solver;
  z3::expr num_points;
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
