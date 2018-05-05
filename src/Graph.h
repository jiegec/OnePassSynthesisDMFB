#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <string>
#include <utility>
#include <vector>
#include "Node.h"

class Graph {
  typedef std::pair<int, int> edge_type;

 public:
  Graph(const char* file);
  void print_to_graphviz(const char* file);

 private:
  std::string name;
  std::vector<edge_type> edges;
  std::vector<Node> nodes;
  int num_output;
  int num_dispenser;

  friend class Solver;
};

#endif
