#include "Node.h"
#include <sstream>

using namespace std;

string Node::to_string() {
    ostringstream out;
    out << label << " " << id;
    return out.str();
}
