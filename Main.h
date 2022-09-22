#pragma once
#include "Node.h"
#include "Token.h"

auto scan(string)->vector<Token>;
auto parse(vector<Token>)->Program*;
auto interpret(Program*)->void;
