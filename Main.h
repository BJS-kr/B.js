#ifndef MAIN
#define MAIN

#include "Node.h"
#include "Token.h"
#include <sstream>
#include <fstream>

using std::stringstream;
using std::ifstream;

auto global(string sourceCode)->string {
    return "function global() {" + sourceCode + "}";
};
auto scan(string)->vector<Token>;
auto parse(vector<Token>)->Program*;
auto interpret(Program*)->void;

#endif