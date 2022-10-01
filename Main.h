#pragma once
#include "Node.h"
#include "Token.h"
#include <sstream>
#include <fstream>

using std::stringstream;
using std::ifstream;

auto mainify(string sourceCode)->string {
    return "function main() {" + sourceCode + "}";
};
auto scan(string)->vector<Token>;
auto parse(vector<Token>)->Program*;
auto interpret(Program*)->void;
