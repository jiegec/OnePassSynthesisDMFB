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
