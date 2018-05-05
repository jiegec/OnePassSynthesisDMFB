#include <iostream>
#include <z3++.h>

#include "Graph.h"
#include "Solver.h"

using namespace z3;
using namespace std;

int main() {
  Graph graph("../../testcase/Single_2_Input_Mix.txt");
  graph.print_to_graphviz("pcr.dot");
  system("dot -Tpng -o pcr.png pcr.dot");
  context c;
  Solver solver(c, graph, 2, 2, 6);
  auto ans = solver.get();
  auto model = ans.get_model();
  solver.print(model);
  return 0;
}
