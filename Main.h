#ifndef MAIN
#define MAIN

#include "Node.h"
#include "Token.h"
#include "Code.h"
#include <sstream>
#include <fstream>

using std::stringstream;
using std::ifstream;
using std::tuple;
auto global(string sourceCode)->string {
    return "function global() {" + sourceCode + "}";
};
auto scan(string)->vector<Token>;
auto parse(vector<Token>)->Program*;
auto generate(Program*)->tuple<vector<Code>, map<string, size_t>>;


#endif