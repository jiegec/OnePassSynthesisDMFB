#include <iostream>
#include <z3++.h>
#include <chrono>

using namespace std::chrono;

#include "Graph.h"
#include "Solver.h"

using namespace z3;
using namespace std;

bool try_steps(const Graph &graph, int n) {
  cout << "Trying step " << n << endl;
  context c;
  auto before = high_resolution_clock::now();
  Solver solver(c, graph, 10, 10, n);
  auto after = high_resolution_clock::now();
  cout << "Used " << duration_cast<milliseconds>(after - before).count() << "ms" << endl;
  auto ans = solver.get();
  if (ans.check() == sat) {
    cout << ans.to_smt2() << endl;
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
