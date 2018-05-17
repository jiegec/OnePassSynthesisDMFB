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

bool try_steps(const Graph &graph, int n) {
  cout << "Trying step " << n << endl;
  context c;
  auto before = high_resolution_clock::now();
  Solver solver(c, graph, 5, 5, n, 0x7fffffff);
  auto ans = solver.get_solver();
  auto result = ans.check();
  auto after = high_resolution_clock::now();
  cout << "Used " << duration_cast<milliseconds>(after - before).count() << "ms"
       << endl;
  cout << ans.to_smt2() << endl;
  if (result != sat) {
    cerr << "Unsatisfiable" << endl;
  } else {
    cout << "Satisfiable. Optimizing num of points." << endl;
    int left = 0, right = 0x7fffffff;
    while (1) {
      int mid = left + ((right - left) >> 1);
      cout << "Trying " << mid << endl;
      Solver temp(c, graph, 5, 5, n, mid);
      auto temp_ans = temp.get_solver();
      if (temp_ans.check() == sat) {
        right = mid;
      } else {
        left = mid + 1;
      }

      if (left >= right) {
        auto model = temp_ans.get_model();
        solver.print(model);
        return true;
      }
    }
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
  for (int i = 1; try_steps(graph, i) == false; i++)
    ;
  return 0;
}
