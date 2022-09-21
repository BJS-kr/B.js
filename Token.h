#include "./kind.h"
#include <string>

using std::string;

string toString(Kind);
Kind toKind(string);

struct Token {
  // unknown by default
  Kind kind = Kind::Unknown;
  string code;
};