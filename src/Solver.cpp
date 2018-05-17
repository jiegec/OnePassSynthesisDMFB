#include "Solver.h"
#include <fstream>
#include <stdio.h>

using namespace z3;
using namespace std;

const int neigh[][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}, {0, 0}};

Solver::Solver(context &ctx, const Graph &graph, int width, int height,
               int time)
    : solver(ctx), width(width), height(height), time(time), graph(graph) {
  char buffer[512];
  expr dummy(ctx);
  c.resize(height);
  dispenser.resize(2 * (height + width));
  sink.resize(2 * (height + width), dummy);
  for (int i = 0; i < 2 * (height + width); i++) {
    dispenser[i].resize(graph.nodes.size(), dummy);
    sprintf(buffer, "sink_p%d", i);
    sink[i] = ctx.bool_const(buffer);
    for (int j = 0; j < graph.nodes.size(); j++) {
      sprintf(buffer, "dispenser_p%d_l%d", i, j);
      dispenser[i][j] = ctx.bool_const(buffer);
    }
  }
  for (int i = 0; i < height; i++) {
    c[i].resize(width);
    for (int j = 0; j < width; j++) {
      c[i][j].resize(graph.nodes.size() + 1);
      for (auto &node : graph.nodes) {
        c[i][j][node.id].resize(time + 1, dummy);
        for (int t = 1; t <= time; t++) {
          if (node.type == DISPENSE || node.type == MIX) {
            sprintf(buffer, "c_x%d_y%d_i%d_t%d", i, j, node.id, t);
            c[i][j][node.id][t] = ctx.bool_const(buffer);
          }
        }
      }
    }
  }

  add_consistency(ctx);
  add_placement(ctx);
  add_movement(ctx);

  cout << solver.to_smt2() << endl;
  if (solver.check() != sat) {
    cerr << "Unsatisfiable" << endl;
  }
}

solver Solver::get() { return solver; }

void Solver::print(const model &model) {
  for (int i = 0; i < graph.nodes.size(); i++) {
    if (graph.nodes[i].type == DISPENSE) {
      for (int j = 0; j < 2 * (width + height); j++) {
        if (model.eval(dispenser[j][i]).bool_value() == Z3_L_TRUE) {
          cout << "Dispenser at " << j << " of type " << i << endl;
        }
      }
    }
  }
  for (int i = 0; i < 2 * (width + height); i++) {
    if (model.eval(sink[i]).bool_value() == Z3_L_TRUE) {
      cout << "Sink at " << i << endl;
    }
  }
  for (int t = 1; t <= time; t++) {
    char file_name[128];
    sprintf(file_name, "time%d.dot", t);
    char img_name[128];
    sprintf(img_name, "time%d.png", t);
    ofstream out(file_name);
    out << "digraph step {rankdir=LR;node "
           "[shape=record,fontname=\"Inconsolata\"];"
        << endl;
    out << "dispenser [label=\"Dispensers:|" << endl;
    bool first_dispenser = true;
    for (int i = 0; i < graph.nodes.size(); i++) {
      if (graph.nodes[i].type == DISPENSE) {
        for (int j = 0; j < 2 * (width + height); j++) {
          if (model.eval(dispenser[j][i]).bool_value() == Z3_L_TRUE) {
            if (first_dispenser) {
              first_dispenser = false;
            } else {
              out << "|";
            }
            out << "<d" << j << ">" << i;
          }
        }
      }
    }
    out << "\"];" << endl;

    out << "sink [label=\"Sinks:|" << endl;
    bool first_sink = true;
    for (int j = 0; j < 2 * (width + height); j++) {
      if (model.eval(sink[j]).bool_value() == Z3_L_TRUE) {
        if (first_sink) {
          first_sink = false;
        } else {
          out << "|";
        }
        out << "<s" << j << ">"
            << "S";
      }
    }
    out << "\"];" << endl;

    out << "board [label=\"" << endl;
    cout << "time: " << t << endl;
    for (int i = 0; i < height; i++) {
      if (i != 0) {
        out << "|";
      }
      out << "{";
      for (int j = 0; j < width; j++) {
        out << "<f" << i << j << ">";

        bool flag = false;
        for (auto &node : graph.nodes) {
          if (node.type == DISPENSE || node.type == MIX) {
            if (model.eval(c[i][j][node.id][t]).bool_value() == Z3_L_TRUE) {
              cout << node.id << " ";
              if (j != width - 1) {
                out << node.id << "|";
              } else {
                out << node.id;
              }
              flag = true;
            }
          }
        }
        if (!flag) {
          cout << "* ";
          if (j != width - 1) {
            out << "E"
                << "|";
          } else {
            out << "E";
          }
        }
      }
      out << "}";
      cout << endl;
    }
    cout << endl;
    out << "\"];" << endl;

    // dispensers
    for (int i = 0; i < graph.nodes.size(); i++) {
      if (graph.nodes[i].type == DISPENSE) {
        for (int j = 0; j < 2 * (width + height); j++) {
          int x = 0, y = 0;
          if (j < width) {
            y = width;
          } else if (j < width + height) {
            x = j - width;
            y = width - 1;
          } else if (j < width * 2 + height) {
            x = height - 1;
            y = width * 2 + height - j - 1;
          } else {
            x = (width + height) * 2 - j - 1;
            y = 0;
          }
          if (model.eval(dispenser[j][i]).bool_value() == Z3_L_TRUE) {
            out << "dispenser:d" << j << " -> board:f" << x << y << endl;
          }
        }
      }
    }

    // sink
    for (int j = 0; j < 2 * (width + height); j++) {
      int x = 0, y = 0;
      if (j < width) {
        y = width;
      } else if (j < width + height) {
        x = j - width;
        y = width - 1;
      } else if (j < width * 2 + height) {
        x = height - 1;
        y = width * 2 + height - j - 1;
      } else {
        x = (width + height) * 2 - j - 1;
        y = 0;
      }
      if (model.eval(sink[j]).bool_value() == Z3_L_TRUE) {
        out << "sink:s" << j << " -> board:f" << x << y << endl;
      }
    }

    out << "}" << endl;
    out.close();
    char cmd_line[128];
    sprintf(cmd_line, "dot -Tpng -o %s %s", img_name, file_name);
    system(cmd_line);
  }
  system("convert -delay 50 -loop 0 time*.png animation.gif");
}

void Solver::add_consistency(context &ctx) {
  // A cell may not be occupied by more than one droplet
  // or mixer i per time step
  expr_vector consistency1_vec(ctx);
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      for (int t = 1; t <= time; t++) {
        expr_vector vec(ctx);
        for (auto &node : graph.nodes) {
          if (node.type == DISPENSE || node.type == MIX) {
            vec.push_back(c[i][j][node.id][t]);
          }
        }
        consistency1_vec.push_back(atmost(vec, 1));
      }
    }
  }
  solver.add(mk_and(consistency1_vec));

  // each droplet i may occur in at most one cell per time
  // step
  expr_vector consistency2_vec(ctx);
  for (auto &node : graph.nodes) {
    if (node.type == DISPENSE || node.type == MIX) {
      for (int t = 1; t <= time; t++) {
        expr_vector vec(ctx);
        for (int i = 0; i < height; i++) {
          for (int j = 0; j < width; j++) {
            vec.push_back(c[i][j][node.id][t]);
          }
        }
        consistency2_vec.push_back(atmost(vec, 1));
      }
    }
  }
  solver.add(mk_and(consistency2_vec));

  // in each position p outside of the grid, there may be at
  // most one dispenser
  expr_vector consistency3_vec(ctx);
  for (int i = 0; i < 2 * (width + height); i++) {
    expr_vector vec(ctx);
    vec.push_back(sink[i]);
    for (int j = 0; j < graph.nodes.size(); j++) {
      if (graph.nodes[j].type == DISPENSE || graph.nodes[j].type == MIX) {
        vec.push_back(dispenser[i][j]);
      }
    }
    consistency3_vec.push_back(atmost(vec, 1));
  }
  solver.add(mk_and(consistency3_vec));
  // each cell may be occupied by at most one detector
  // ignore detector for now

  // each droplet i should occur in at least one time
  expr_vector consistency4_vec(ctx);
  for (auto &node : graph.nodes) {
    if (node.type == DISPENSE || node.type == MIX) {
      expr_vector vec(ctx);
      for (int t = 1; t <= time; t++) {
        for (int i = 0; i < height; i++) {
          for (int j = 0; j < width; j++) {
            vec.push_back(c[i][j][node.id][t]);
          }
        }
      }
      consistency4_vec.push_back(mk_or(vec));
    }
  }
  solver.add(mk_and(consistency4_vec));
}

void Solver::add_placement(context &ctx) {
  // For detectors, we ensure that, over all possible (x,y)- cells, for every
  // type l of fluids a detector is placed
  // ignore detector for now

  // For dispensers and sinks, we proceed analogously: For
  // every possible outside position p of the grid and every type of fluid l,
  // we ensure that the desired amount of entities
  expr_vector placement2_vec(ctx);
  for (int i = 0; i < graph.nodes.size(); i++) {
    if (graph.nodes[i].type == DISPENSE || graph.nodes[i].type == MIX) {
      expr_vector vec(ctx);
      for (int j = 0; j < 2 * (height + width); j++) {
        vec.push_back(dispenser[j][i]);
      }
      auto n_dispensers = graph.nodes[i].type == DISPENSE ? 1 : 0;
      placement2_vec.push_back(atmost(vec, n_dispensers));
      placement2_vec.push_back(atleast(vec, n_dispensers));
    }
  }
  expr_vector sink_vec(ctx);
  for (int i = 0; i < 2 * (height + width); i++) {
    sink_vec.push_back(sink[i]);
  }
  placement2_vec.push_back(atmost(sink_vec, graph.num_output));
  placement2_vec.push_back(atleast(sink_vec, graph.num_output));
  solver.add(mk_and(placement2_vec));
}

void Solver::add_movement(context &ctx) {
  expr_vector movement(ctx);
  for (int i = 0; i < graph.nodes.size(); i++) {
    if (graph.nodes[i].type == DISPENSE || graph.nodes[i].type == MIX) {
      for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
          for (int t = 1; t <= time; t++) {
            expr_vector vec(ctx);
            // from neighbour last time
            if (t > 1) {
              for (int d = 0; d < 5; d++) {
                int xx = x + neigh[d][0];
                int yy = y + neigh[d][1];
                if (0 <= xx && 0 <= yy && xx < height && yy < width) {
                  vec.push_back(c[xx][yy][graph.nodes[i].id][t - 1]);
                }
              }
            }

            // if it is poured from dispenser
            if (graph.nodes[i].type == DISPENSE) {
              if (x == 0) {
                vec.push_back(dispenser[y][graph.nodes[i].id]);
              }
              if (y == 0) {
                vec.push_back(
                    dispenser[2 * (width + height) - x - 1][graph.nodes[i].id]);
              }
              if (x == height - 1) {
                vec.push_back(
                    dispenser[2 * width + height - y - 1][graph.nodes[i].id]);
              }
              if (y == width - 1) {
                vec.push_back(dispenser[width + x][graph.nodes[i].id]);
              }
            }

            // If it is an output from a MIX operation
            if (graph.nodes[i].type == MIX) {
              if (t >= graph.nodes[i].time + 2) {
                const int mix_width = 2, mix_height = 2;
                expr_vector mix_vec(ctx);
                for (auto &edges : graph.edges) {
                  if (edges.second == i) {
                    // edges.first is an input liquid
                    expr_vector appear_before_mix(ctx);
                    expr_vector disappear_on_mix(ctx);
                    for (int dir = 0; dir < 5; dir++) {
                      int new_x = x + neigh[dir][0];
                      int new_y = y + neigh[dir][1];
                      if (0 <= new_x && new_x < height && 0 <= new_y &&
                          new_y < width) {
                        appear_before_mix.push_back(
                            c[new_x][new_y][edges.first]
                             [t - graph.nodes[i].time - 1]);
                        disappear_on_mix.push_back(c[new_x][new_y][edges.first]
                                                    [t - graph.nodes[i].time]);
                      }
                    }
                    // the liquid appears in the neighbour before mix
                    mix_vec.push_back(mk_or(appear_before_mix));
                    // the liquid disappears after mix
                    mix_vec.push_back(not(mk_or(disappear_on_mix)));
                  }
                }
                // TODO: occupy the space when mixing
                vec.push_back(mk_and(mix_vec));
              } else {
                // should never appear
                vec.push_back(ctx.bool_val(false));
              }
            }
            if (vec.size() > 0)
              movement.push_back(
                  implies(c[x][y][graph.nodes[i].id][t], mk_or(vec)));
          }
        }
      }
    }
  }
  solver.add(mk_and(movement));

  // OUTPUT: liquid should be output to sink
  for (int i = 0; i < graph.nodes.size(); i++) {
    if (graph.nodes[i].type == OUTPUT) {
      expr_vector mix_vec(ctx);
      for (auto &edges : graph.edges) {
        if (edges.second == i) {
          // edges.first: the liquid to output
          for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
              for (int t = 2; t <= time; t++) {
                expr_vector vec(ctx);
                // disappear at time t
                for (int d = 0; d < 5; d++) {
                  int xx = x + neigh[d][0];
                  int yy = y + neigh[d][1];
                  if (0 <= xx && 0 <= yy && xx < height && yy < width) {
                    vec.push_back(c[xx][yy][edges.first][t]);
                  }
                }

                auto disappear_at_t = not(mk_or(vec));
                auto disappear =
                    (c[x][y][edges.first][t - 1] && disappear_at_t);
                expr_vector adj_sink(ctx);

                if (x == 0) {
                  adj_sink.push_back(sink[y]);
                }
                if (y == 0) {
                  adj_sink.push_back(sink[2 * (width + height) - x - 1]);
                }
                if (x == height - 1) {
                  adj_sink.push_back(sink[2 * width + height - y - 1]);
                }
                if (y == width - 1) {
                  adj_sink.push_back(sink[width + x]);
                }
                solver.add(implies(disappear, mk_or(adj_sink)));

                // edges.first should not appear at the last time
                expr_vector disappear_last(ctx);
                for (int x = 0; x < height; x++) {
                  for (int y = 0; y < width; y++) {
                    disappear_last.push_back(c[x][y][edges.first][time]);
                  }
                }
                solver.add(not(mk_or(disappear_last)));
              }
            }
          }
          break;
        }
      }
    }
  }
}