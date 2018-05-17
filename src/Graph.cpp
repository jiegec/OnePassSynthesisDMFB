#include "Graph.h"
#include <fstream>
#include <string.h>

using namespace std;

void trim(std::string &s) {
  if (!s.empty()) {
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
  }
}

Graph::Graph(const char *file) {
  ifstream in(file);
  this->num_output = this->num_dispenser = 0;
  while (!in.eof()) {
    string line;
    getline(in, line);
    if (line.find('(') == string::npos)
      continue;

    string name(line, 0, line.find('('));
    trim(name);

    auto begin = line.find('(') + 1, next = string::npos;
    vector<string> params;
    while ((next = line.find_first_of(",)", begin)) != string::npos) {
      string param(line, begin, next - begin);
      trim(param);
      params.emplace_back(move(param));
      begin = next + 1;
    }

    if (name == "DAGNAME") {
      this->name = params[0];
    } else if (name == "EDGE") {
      auto from = stoi(params[0]), to = stoi(params[1]);
      this->edges.emplace_back(from - 1, to - 1);
    } else if (name == "NODE") {
      Node node;
      node.id = stoi(params[0]) - 1;
      auto type = params[1];
      if (type == "DISPENSE") {
        node.type = DISPENSE;
        node.fluid_name = params[2];
        node.volume = stoi(params[3]);
        node.label = params[4];
        this->num_dispenser++;
      } else if (type == "MIX") {
        node.type = MIX;
        node.drops = stoi(params[2]);
        node.time = stoi(params[3]);
        node.label = params[4];
      } else if (type == "OUTPUT") {
        node.type = OUTPUT;
        node.sink_name = params[2];
        node.label = params[3];
        this->num_output++;
      } else if (type == "DETECT") {
        node.type = DETECT;
        // unfinished
      }
      this->nodes.emplace_back(move(node));
    }
  }
}

void Graph::print_to_graphviz(const char *file) {
  ofstream out(file);
  out << "graph \"" << this->name << "\" {" << endl;
  for (auto &node : this->nodes) {
    out << node.id << " [label=\"" << node.to_string() << "\"]" << endl;
  }
  for (auto &edge : this->edges) {
    out << edge.first << " -- " << edge.second << endl;
  }
  out << "}" << endl;
}
