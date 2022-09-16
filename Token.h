#include "./kind.h"
#include <string>

using std::string;

string toString(Kind);
Kind toKind(Kind);

struct Token {
  Kind kind = Kind::Unknown;
  string string;
};