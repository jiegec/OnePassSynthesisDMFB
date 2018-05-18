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

#include <chrono>
#include <iostream>
#include <z3++.h>

using namespace std::chrono;

#include "Graph.h"
#include "Solver.h"

using namespace z3;
using namespace std;

bool try_steps(const Graph &graph, int width, int height, int n) {
  try {
    cout << "Trying step " << n << endl;
    context c;
    auto before = high_resolution_clock::now();
    Solver solver(c, graph, width, height, n);
    auto& ans = solver.get_solver();
    auto result = ans.check();
    auto after = high_resolution_clock::now();
    cout << "Used " << duration_cast<milliseconds>(after - before).count()
         << "ms" << endl;
    if (result != sat) {
      cout << "Unsatisfiable" << endl;
    } else {
      cout << "Satisfiable" << endl;
      auto model = ans.get_model();
      solver.print(model);
      return true;
    }
  } catch (z3::exception e) {
    cerr << e.msg() << endl;
    return true;
  }
  return false;
}

int main(int argc, char **argv) {
  const char *filename = "../../testcase/Assays/Testing/Single_2_Input_Mix.txt";
  if (argc > 1) {
    filename = argv[1];
  }
  Graph graph(filename);
  graph.print_to_graphviz("input.dot");
  system("dot -Tpng -o input.png input.dot");
  // try_steps(graph, 6);
  for (int i = 1; try_steps(graph, 5, 5, i) == false; i++)
    ;
  return 0;
}
