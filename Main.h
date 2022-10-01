#pragma once
#include "Node.h"
#include "Token.h"

auto mainify(string sourceCode)->string {
    return "function main() {" + sourceCode + "}";
};
auto scan(string)->vector<Token>;
auto parse(vector<Token>)->Program*;
auto interpret(Program*)->void;
