#ifndef __NODE_H__
#define __NODE_H__

#include <string>

enum NodeType { INVALID, DISPENSE, MIX, OUTPUT, DETECT };

struct Node {
  int id;
  NodeType type;
  std::string label;

  // MIX
  int time;
  int drops;

  // DISPENSE
  std::string fluid_name;
  int volume;

  // OUTPUT
  std::string sink_name;

  std::string to_string();
};

#endif
