#include <iostream>
#include <z3++.h>

#include "Graph.h"
#include "Solver.h"

using namespace z3;
using namespace std;

bool try_steps(const Graph &graph, int n) {
  context c;
  Solver solver(c, graph, 10, 10, n);
  auto ans = solver.get();
  if (ans.check() == sat) {
    auto model = ans.get_model();
    solver.print(model);
    return true;
  }
  return false;
}

int main(int argc, char **argv) {
  const char *filename = "../../testcase/Single_2_Input_Mix.txt";
  if (argc > 1) {
    filename = argv[1];
  }
  Graph graph(filename);
  graph.print_to_graphviz("input.dot");
  system("dot -Tpng -o input.png input.dot");
  for (int i = 0;try_steps(graph, i) == false;i++);
  return 0;
}
